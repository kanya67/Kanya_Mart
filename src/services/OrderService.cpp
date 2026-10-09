#include "services/OrderService.h"
#include "utils/Response.h"
#include <drogon/drogon.h>

using namespace drogon;

void OrderService::createOrder(const std::string &userId,
                                std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();

    // Use a transaction for atomic order creation
    auto transPtr = dbClient->newTransaction();

    // Step 1: Get cart items with product info
    transPtr->execSqlAsync(
        "SELECT ci.product_id, ci.quantity, "
        "p.name, p.price, p.stock, p.seller_id, p.is_active "
        "FROM cart_items ci "
        "JOIN products p ON ci.product_id = p.id "
        "WHERE ci.user_id = $1 "
        "FOR UPDATE OF p",  // Lock product rows
        [transPtr, userId, callback](const orm::Result &cartResult) {
            if (cartResult.empty()) {
                callback(400, Response::error("EMPTY_CART", "Cart is empty"));
                return;
            }

            // Verify all products and calculate total
            double total = 0;
            Json::Value orderItems(Json::arrayValue);
            for (const auto &row : cartResult) {
                bool isActive = row["is_active"].as<bool>();
                int stock = row["stock"].as<int>();
                int quantity = row["quantity"].as<int>();

                if (!isActive) {
                    callback(400, Response::error("PRODUCT_UNAVAILABLE",
                        "Product '" + row["name"].as<std::string>() + "' is no longer available"));
                    return;
                }

                if (quantity > stock) {
                    callback(400, Response::error("INSUFFICIENT_STOCK",
                        "Insufficient stock for '" + row["name"].as<std::string>() +
                        "'. Available: " + std::to_string(stock)));
                    return;
                }

                double subtotal = row["price"].as<double>() * quantity;
                total += subtotal;

                Json::Value item;
                item["product_id"] = row["product_id"].as<std::string>();
                item["seller_id"] = row["seller_id"].as<std::string>();
                item["product_name"] = row["name"].as<std::string>();
                item["quantity"] = quantity;
                item["unit_price"] = row["price"].as<double>();
                item["subtotal"] = subtotal;
                item["stock"] = stock;
                orderItems.append(item);
            }

            // Step 2: Create order
            transPtr->execSqlAsync(
                "INSERT INTO orders (user_id, total, status) "
                "VALUES ($1, $2, 'PENDING') "
                "RETURNING id, to_char(created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at",
                [transPtr, userId, total, orderItems, callback](const orm::Result &orderResult) {
                    std::string orderId = orderResult[0]["id"].as<std::string>();
                    std::string createdAt = orderResult[0]["created_at"].as<std::string>();

                    // Step 3: Insert order items and update stock
                    int itemCount = orderItems.size();
                    auto remaining = std::make_shared<int>(itemCount * 2); // items + stock updates
                    auto failed = std::make_shared<bool>(false);
                    auto orderIdPtr = std::make_shared<std::string>(orderId);
                    auto createdAtPtr = std::make_shared<std::string>(createdAt);
                    auto totalPtr = std::make_shared<double>(total);
                    auto itemsPtr = std::make_shared<Json::Value>(orderItems);

                    for (int i = 0; i < itemCount; i++) {
                        const auto &item = orderItems[i];
                        std::string prodId = item["product_id"].asString();
                        std::string sellerId = item["seller_id"].asString();
                        std::string prodName = item["product_name"].asString();
                        int qty = item["quantity"].asInt();
                        double unitPrice = item["unit_price"].asDouble();
                        double subtotal = item["subtotal"].asDouble();

                        // Insert order item
                        transPtr->execSqlAsync(
                            "INSERT INTO order_items (order_id, product_id, seller_id, product_name, quantity, unit_price, subtotal) "
                            "VALUES ($1, $2, $3, $4, $5, $6, $7)",
                            [remaining, failed, transPtr, userId, orderIdPtr, createdAtPtr, totalPtr, itemsPtr, callback](const orm::Result &) {
                                (*remaining)--;
                                if (*remaining == 0 && !*failed) {
                                    // Step 4: Clear cart
                                    transPtr->execSqlAsync(
                                        "DELETE FROM cart_items WHERE user_id = $1",
                                        [callback, orderIdPtr, createdAtPtr, totalPtr, itemsPtr](const orm::Result &) {
                                            Json::Value order;
                                            order["id"] = *orderIdPtr;
                                            order["total"] = *totalPtr;
                                            order["status"] = "PENDING";
                                            order["created_at"] = *createdAtPtr;
                                            order["items"] = *itemsPtr;
                                            order["message"] = "Order created successfully";
                                            callback(201, Response::success(order));
                                        },
                                        [callback](const orm::DrogonDbException &e) {
                                            callback(500, Response::error("SERVER_ERROR", "Failed to clear cart"));
                                        },
                                        userId
                                    );
                                }
                            },
                            [remaining, failed, callback](const orm::DrogonDbException &e) {
                                if (!*failed) {
                                    *failed = true;
                                    callback(500, Response::error("SERVER_ERROR", "Failed to create order items"));
                                }
                            },
                            orderId, prodId, sellerId, prodName, qty, unitPrice, subtotal
                        );

                        // Update product stock
                        transPtr->execSqlAsync(
                            "UPDATE products SET stock = stock - $2, updated_at = NOW() WHERE id = $1",
                            [remaining, failed, transPtr, userId, orderIdPtr, createdAtPtr, totalPtr, itemsPtr, callback](const orm::Result &) {
                                (*remaining)--;
                                if (*remaining == 0 && !*failed) {
                                    // Step 4: Clear cart
                                    transPtr->execSqlAsync(
                                        "DELETE FROM cart_items WHERE user_id = $1",
                                        [callback, orderIdPtr, createdAtPtr, totalPtr, itemsPtr](const orm::Result &) {
                                            Json::Value order;
                                            order["id"] = *orderIdPtr;
                                            order["total"] = *totalPtr;
                                            order["status"] = "PENDING";
                                            order["created_at"] = *createdAtPtr;
                                            order["items"] = *itemsPtr;
                                            order["message"] = "Order created successfully";
                                            callback(201, Response::success(order));
                                        },
                                        [callback](const orm::DrogonDbException &e) {
                                            callback(500, Response::error("SERVER_ERROR", "Failed to clear cart"));
                                        },
                                        userId
                                    );
                                }
                            },
                            [remaining, failed, callback](const orm::DrogonDbException &e) {
                                if (!*failed) {
                                    *failed = true;
                                    callback(500, Response::error("SERVER_ERROR", "Failed to update stock"));
                                }
                            },
                            prodId, qty
                        );
                    }
                },
                [callback](const orm::DrogonDbException &e) {
                    callback(500, Response::error("SERVER_ERROR", "Failed to create order"));
                },
                userId, total
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to fetch cart"));
        },
        userId
    );
}

void OrderService::getOrders(const std::string &userId,
                              std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT o.id, o.total, o.status, "
        "to_char(o.created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
        "to_char(o.updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at, "
        "COUNT(oi.id) as item_count "
        "FROM orders o "
        "LEFT JOIN order_items oi ON o.id = oi.order_id "
        "WHERE o.user_id = $1 "
        "GROUP BY o.id "
        "ORDER BY o.created_at DESC",
        [callback](const orm::Result &result) {
            Json::Value orders(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value order;
                order["id"] = row["id"].as<std::string>();
                order["total"] = row["total"].as<double>();
                order["status"] = row["status"].as<std::string>();
                order["created_at"] = row["created_at"].as<std::string>();
                order["updated_at"] = row["updated_at"].as<std::string>();
                order["item_count"] = row["item_count"].as<int>();
                orders.append(order);
            }
            callback(200, Response::success(orders));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to fetch orders"));
        },
        userId
    );
}

void OrderService::getOrder(const std::string &userId, const std::string &orderId,
                             std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT o.id, o.total, o.status, o.user_id, "
        "to_char(o.created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
        "to_char(o.updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at "
        "FROM orders o WHERE o.id = $1",
        [callback, dbClient, userId, orderId](const orm::Result &result) {
            if (result.empty()) {
                callback(404, Response::error("ORDER_NOT_FOUND", "Order not found"));
                return;
            }

            // Security: only owner can view
            if (result[0]["user_id"].as<std::string>() != userId) {
                callback(403, Response::error("FORBIDDEN", "Access denied"));
                return;
            }

            Json::Value order;
            order["id"] = result[0]["id"].as<std::string>();
            order["total"] = result[0]["total"].as<double>();
            order["status"] = result[0]["status"].as<std::string>();
            order["created_at"] = result[0]["created_at"].as<std::string>();
            order["updated_at"] = result[0]["updated_at"].as<std::string>();

            // Fetch order items
            dbClient->execSqlAsync(
                "SELECT oi.id, oi.product_id, oi.seller_id, oi.product_name, "
                "oi.quantity, oi.unit_price, oi.subtotal, "
                "p.image_url, u.username as seller_name "
                "FROM order_items oi "
                "LEFT JOIN products p ON oi.product_id = p.id "
                "LEFT JOIN users u ON oi.seller_id = u.id "
                "WHERE oi.order_id = $1",
                [callback, order](const orm::Result &itemResult) mutable {
                    Json::Value items(Json::arrayValue);
                    for (const auto &row : itemResult) {
                        Json::Value item;
                        item["id"] = row["id"].as<std::string>();
                        item["product_id"] = row["product_id"].as<std::string>();
                        item["seller_id"] = row["seller_id"].as<std::string>();
                        item["product_name"] = row["product_name"].as<std::string>();
                        item["quantity"] = row["quantity"].as<int>();
                        item["unit_price"] = row["unit_price"].as<double>();
                        item["subtotal"] = row["subtotal"].as<double>();
                        item["image_url"] = row["image_url"].isNull() ? "" : row["image_url"].as<std::string>();
                        item["seller_name"] = row["seller_name"].isNull() ? "" : row["seller_name"].as<std::string>();
                        items.append(item);
                    }
                    order["items"] = items;
                    callback(200, Response::success(order));
                },
                [callback, order](const orm::DrogonDbException &e) mutable {
                    order["items"] = Json::Value(Json::arrayValue);
                    callback(200, Response::success(order));
                },
                orderId
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to fetch order"));
        },
        orderId
    );
}
