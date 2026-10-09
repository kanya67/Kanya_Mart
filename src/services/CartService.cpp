#include "services/CartService.h"
#include "utils/Response.h"
#include <drogon/drogon.h>

using namespace drogon;

void CartService::getCart(const std::string &userId,
                           std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT ci.id, ci.product_id, ci.quantity, "
        "p.name, p.price, p.stock, p.image_url, p.category, p.is_active, "
        "u.username as seller_name "
        "FROM cart_items ci "
        "JOIN products p ON ci.product_id = p.id "
        "LEFT JOIN users u ON p.seller_id = u.id "
        "WHERE ci.user_id = $1 "
        "ORDER BY ci.created_at DESC",
        [callback](const orm::Result &result) {
            Json::Value items(Json::arrayValue);
            double total = 0;
            for (const auto &row : result) {
                Json::Value item;
                item["id"] = row["id"].as<std::string>();
                item["product_id"] = row["product_id"].as<std::string>();
                item["quantity"] = row["quantity"].as<int>();
                item["name"] = row["name"].as<std::string>();
                item["price"] = row["price"].as<double>();
                item["stock"] = row["stock"].as<int>();
                item["image_url"] = row["image_url"].as<std::string>();
                item["category"] = row["category"].as<std::string>();
                item["is_active"] = row["is_active"].as<bool>();
                item["seller_name"] = row["seller_name"].as<std::string>();
                double subtotal = row["price"].as<double>() * row["quantity"].as<int>();
                item["subtotal"] = subtotal;
                total += subtotal;
                items.append(item);
            }
            Json::Value cart;
            cart["items"] = items;
            cart["item_count"] = static_cast<int>(items.size());
            cart["total"] = total;
            callback(200, Response::success(cart));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to fetch cart"));
        },
        userId
    );
}

void CartService::addItem(const std::string &userId, const std::string &productId, int quantity,
                           std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();

    // Verify product exists and is active
    dbClient->execSqlAsync(
        "SELECT id, stock, is_active, price, name FROM products WHERE id = $1",
        [dbClient, userId, productId, quantity, callback](const orm::Result &result) {
            if (result.empty()) {
                callback(404, Response::error("PRODUCT_NOT_FOUND", "Product not found"));
                return;
            }
            bool isActive = result[0]["is_active"].as<bool>();
            int stock = result[0]["stock"].as<int>();

            if (!isActive) {
                callback(400, Response::error("PRODUCT_UNAVAILABLE", "Product is not available"));
                return;
            }

            // Check existing cart item
            dbClient->execSqlAsync(
                "SELECT id, quantity FROM cart_items WHERE user_id = $1 AND product_id = $2",
                [dbClient, userId, productId, quantity, stock, callback](const orm::Result &cartResult) {
                    int newQty = quantity;
                    if (!cartResult.empty()) {
                        newQty = cartResult[0]["quantity"].as<int>() + quantity;
                    }

                    if (newQty > stock) {
                        callback(400, Response::error("INSUFFICIENT_STOCK",
                            "Only " + std::to_string(stock) + " items available"));
                        return;
                    }

                    // Upsert cart item
                    dbClient->execSqlAsync(
                        "INSERT INTO cart_items (user_id, product_id, quantity) "
                        "VALUES ($1, $2, $3) "
                        "ON CONFLICT (user_id, product_id) "
                        "DO UPDATE SET quantity = $3 "
                        "RETURNING id, product_id, quantity",
                        [callback](const orm::Result &upsertResult) {
                            Json::Value item;
                            item["id"] = upsertResult[0]["id"].as<std::string>();
                            item["product_id"] = upsertResult[0]["product_id"].as<std::string>();
                            item["quantity"] = upsertResult[0]["quantity"].as<int>();
                            item["message"] = "Added to cart";
                            callback(200, Response::success(item));
                        },
                        [callback](const orm::DrogonDbException &e) {
                            callback(500, Response::error("SERVER_ERROR", "Failed to add to cart"));
                        },
                        userId, productId, newQty
                    );
                },
                [callback](const orm::DrogonDbException &e) {
                    callback(500, Response::error("SERVER_ERROR", "Database error"));
                },
                userId, productId
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Database error"));
        },
        productId
    );
}

void CartService::updateItem(const std::string &userId, const std::string &productId, int quantity,
                              std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();

    // Check stock
    dbClient->execSqlAsync(
        "SELECT stock FROM products WHERE id = $1 AND is_active = true",
        [dbClient, userId, productId, quantity, callback](const orm::Result &result) {
            if (result.empty()) {
                callback(404, Response::error("PRODUCT_NOT_FOUND", "Product not found"));
                return;
            }
            int stock = result[0]["stock"].as<int>();
            if (quantity > stock) {
                callback(400, Response::error("INSUFFICIENT_STOCK",
                    "Only " + std::to_string(stock) + " items available"));
                return;
            }

            dbClient->execSqlAsync(
                "UPDATE cart_items SET quantity = $3 "
                "WHERE user_id = $1 AND product_id = $2 "
                "RETURNING id, product_id, quantity",
                [callback](const orm::Result &updateResult) {
                    if (updateResult.empty()) {
                        callback(404, Response::error("CART_ITEM_NOT_FOUND", "Item not in cart"));
                        return;
                    }
                    Json::Value item;
                    item["id"] = updateResult[0]["id"].as<std::string>();
                    item["product_id"] = updateResult[0]["product_id"].as<std::string>();
                    item["quantity"] = updateResult[0]["quantity"].as<int>();
                    item["message"] = "Cart updated";
                    callback(200, Response::success(item));
                },
                [callback](const orm::DrogonDbException &e) {
                    callback(500, Response::error("SERVER_ERROR", "Failed to update cart"));
                },
                userId, productId, quantity
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Database error"));
        },
        productId
    );
}

void CartService::removeItem(const std::string &userId, const std::string &productId,
                              std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "DELETE FROM cart_items WHERE user_id = $1 AND product_id = $2",
        [callback](const orm::Result &result) {
            Json::Value msg;
            msg["message"] = "Item removed from cart";
            callback(200, Response::success(msg));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to remove item"));
        },
        userId, productId
    );
}
