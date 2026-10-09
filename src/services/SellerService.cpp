#include "services/SellerService.h"
#include "utils/Response.h"
#include "utils/Validator.h"
#include <drogon/drogon.h>
#include <map>
#include <set>

using namespace drogon;

// Valid order status transitions
static const std::map<std::string, std::set<std::string>> validTransitions = {
    {"PENDING", {"CONFIRMED", "CANCELLED"}},
    {"CONFIRMED", {"PROCESSING", "CANCELLED"}},
    {"PROCESSING", {"SHIPPED", "CANCELLED"}},
    {"SHIPPED", {"OUT_FOR_DELIVERY", "CANCELLED"}},
    {"OUT_FOR_DELIVERY", {"DELIVERED"}},
    {"DELIVERED", {}},
    {"CANCELLED", {}}
};

void SellerService::getDashboard(const std::string &sellerId,
                                  std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();

    // Get product stats
    dbClient->execSqlAsync(
        "SELECT "
        "COUNT(*) as total_products, "
        "COUNT(*) FILTER (WHERE is_active = true) as active_products, "
        "COUNT(*) FILTER (WHERE stock < 10 AND is_active = true) as low_stock "
        "FROM products WHERE seller_id = $1",
        [dbClient, sellerId, callback](const orm::Result &prodResult) {
            Json::Value dashboard;
            if (!prodResult.empty()) {
                dashboard["total_products"] = prodResult[0]["total_products"].as<int>();
                dashboard["active_products"] = prodResult[0]["active_products"].as<int>();
                dashboard["low_stock"] = prodResult[0]["low_stock"].as<int>();
            }

            // Get order stats
            dbClient->execSqlAsync(
                "SELECT "
                "COUNT(DISTINCT o.id) as total_orders, "
                "COUNT(DISTINCT o.id) FILTER (WHERE o.status = 'PENDING') as pending_orders, "
                "COUNT(DISTINCT o.id) FILTER (WHERE o.status = 'PROCESSING') as processing_orders, "
                "COUNT(DISTINCT o.id) FILTER (WHERE o.status = 'SHIPPED') as shipped_orders, "
                "COUNT(DISTINCT o.id) FILTER (WHERE o.status = 'DELIVERED') as delivered_orders, "
                "COALESCE(SUM(oi.subtotal), 0) as total_sales "
                "FROM order_items oi "
                "JOIN orders o ON oi.order_id = o.id "
                "WHERE oi.seller_id = $1",
                [callback, dashboard](const orm::Result &orderResult) mutable {
                    if (!orderResult.empty()) {
                        dashboard["total_orders"] = orderResult[0]["total_orders"].as<int>();
                        dashboard["pending_orders"] = orderResult[0]["pending_orders"].as<int>();
                        dashboard["processing_orders"] = orderResult[0]["processing_orders"].as<int>();
                        dashboard["shipped_orders"] = orderResult[0]["shipped_orders"].as<int>();
                        dashboard["delivered_orders"] = orderResult[0]["delivered_orders"].as<int>();
                        dashboard["total_sales"] = orderResult[0]["total_sales"].as<double>();
                    }
                    callback(200, Response::success(dashboard));
                },
                [callback, dashboard](const orm::DrogonDbException &e) mutable {
                    dashboard["total_orders"] = 0;
                    dashboard["total_sales"] = 0.0;
                    callback(200, Response::success(dashboard));
                },
                sellerId
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to fetch dashboard"));
        },
        sellerId
    );
}

void SellerService::getProducts(const std::string &sellerId,
                                 std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT p.id, p.name, p.description, p.category, p.price, p.stock, "
        "p.image_url, p.is_active, "
        "to_char(p.created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
        "to_char(p.updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at, "
        "COALESCE(AVG(r.rating), 0) as avg_rating, "
        "COUNT(r.id) as review_count "
        "FROM products p "
        "LEFT JOIN reviews r ON p.id = r.product_id "
        "WHERE p.seller_id = $1 "
        "GROUP BY p.id "
        "ORDER BY p.created_at DESC",
        [callback](const orm::Result &result) {
            Json::Value products(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value p;
                p["id"] = row["id"].as<std::string>();
                p["name"] = row["name"].as<std::string>();
                p["description"] = row["description"].as<std::string>();
                p["category"] = row["category"].as<std::string>();
                p["price"] = row["price"].as<double>();
                p["stock"] = row["stock"].as<int>();
                p["image_url"] = row["image_url"].as<std::string>();
                p["is_active"] = row["is_active"].as<bool>();
                p["created_at"] = row["created_at"].as<std::string>();
                p["updated_at"] = row["updated_at"].as<std::string>();
                p["avg_rating"] = row["avg_rating"].as<double>();
                p["review_count"] = row["review_count"].as<int>();
                products.append(p);
            }
            callback(200, Response::success(products));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to fetch products"));
        },
        sellerId
    );
}

void SellerService::createProduct(const std::string &sellerId, const Json::Value &data,
                                   std::function<void(int, Json::Value)> callback) {
    std::string name = Validator::trim(data.get("name", "").asString());
    std::string description = data.get("description", "").asString();
    std::string category = data.get("category", "").asString();
    double price = data.get("price", 0).asDouble();
    int stock = data.get("stock", 0).asInt();
    std::string imageUrl = data.get("image_url", "").asString();

    if (name.empty()) {
        callback(400, Response::error("VALIDATION_ERROR", "Product name is required"));
        return;
    }
    if (category.empty() || !Validator::isValidCategory(category)) {
        callback(400, Response::error("VALIDATION_ERROR", "Valid category is required"));
        return;
    }
    if (!Validator::isValidPrice(price)) {
        callback(400, Response::error("VALIDATION_ERROR", "Price must be greater than 0"));
        return;
    }
    if (stock < 0) {
        callback(400, Response::error("VALIDATION_ERROR", "Stock cannot be negative"));
        return;
    }

    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "INSERT INTO products (seller_id, name, description, category, price, stock, image_url) "
        "VALUES ($1, $2, $3, $4, $5, $6, $7) "
        "RETURNING id, name, description, category, price, stock, image_url, is_active, "
        "to_char(created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at",
        [callback](const orm::Result &result) {
            Json::Value p;
            p["id"] = result[0]["id"].as<std::string>();
            p["name"] = result[0]["name"].as<std::string>();
            p["description"] = result[0]["description"].as<std::string>();
            p["category"] = result[0]["category"].as<std::string>();
            p["price"] = result[0]["price"].as<double>();
            p["stock"] = result[0]["stock"].as<int>();
            p["image_url"] = result[0]["image_url"].as<std::string>();
            p["is_active"] = result[0]["is_active"].as<bool>();
            p["created_at"] = result[0]["created_at"].as<std::string>();
            p["message"] = "Product created successfully";
            callback(201, Response::success(p));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to create product"));
        },
        sellerId, name, description, category, price, stock, imageUrl
    );
}

void SellerService::updateProduct(const std::string &sellerId, const std::string &productId,
                                   const Json::Value &data,
                                   std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();

    // Verify ownership
    dbClient->execSqlAsync(
        "SELECT id FROM products WHERE id = $1 AND seller_id = $2",
        [dbClient, data, productId, callback](const orm::Result &result) {
            if (result.empty()) {
                callback(404, Response::error("PRODUCT_NOT_FOUND", "Product not found or access denied"));
                return;
            }

            std::string name = data.get("name", "").asString();
            std::string description = data.get("description", "").asString();
            std::string category = data.get("category", "").asString();
            double price = data.get("price", 0).asDouble();
            int stock = data.get("stock", 0).asInt();
            std::string imageUrl = data.get("image_url", "").asString();
            bool isActive = data.get("is_active", true).asBool();

            if (!name.empty() && !category.empty()) {
                if (!Validator::isValidCategory(category)) {
                    callback(400, Response::error("VALIDATION_ERROR", "Invalid category"));
                    return;
                }
                if (price > 0) {
                    dbClient->execSqlAsync(
                        "UPDATE products SET name=$2, description=$3, category=$4, "
                        "price=$5, stock=$6, image_url=$7, is_active=$8, updated_at=NOW() "
                        "WHERE id=$1 "
                        "RETURNING id, name, description, category, price, stock, image_url, is_active, "
                        "to_char(updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at",
                        [callback](const orm::Result &updateResult) {
                            Json::Value p;
                            p["id"] = updateResult[0]["id"].as<std::string>();
                            p["name"] = updateResult[0]["name"].as<std::string>();
                            p["description"] = updateResult[0]["description"].as<std::string>();
                            p["category"] = updateResult[0]["category"].as<std::string>();
                            p["price"] = updateResult[0]["price"].as<double>();
                            p["stock"] = updateResult[0]["stock"].as<int>();
                            p["image_url"] = updateResult[0]["image_url"].as<std::string>();
                            p["is_active"] = updateResult[0]["is_active"].as<bool>();
                            p["updated_at"] = updateResult[0]["updated_at"].as<std::string>();
                            p["message"] = "Product updated successfully";
                            callback(200, Response::success(p));
                        },
                        [callback](const orm::DrogonDbException &e) {
                            callback(500, Response::error("SERVER_ERROR", "Failed to update product"));
                        },
                        productId, name, description, category, price, stock, imageUrl, isActive
                    );
                } else {
                    callback(400, Response::error("VALIDATION_ERROR", "Price must be greater than 0"));
                }
            } else {
                callback(400, Response::error("VALIDATION_ERROR", "Name and category are required"));
            }
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Database error"));
        },
        productId, sellerId
    );
}

void SellerService::deleteProduct(const std::string &sellerId, const std::string &productId,
                                   std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    // Deactivate instead of hard delete
    dbClient->execSqlAsync(
        "UPDATE products SET is_active = false, updated_at = NOW() "
        "WHERE id = $1::uuid AND (seller_id = $2::uuid OR EXISTS (SELECT 1 FROM users WHERE id = $2::uuid AND role = 'admin')) "
        "RETURNING id",
        [callback](const orm::Result &result) {
            if (result.empty()) {
                callback(404, Response::error("PRODUCT_NOT_FOUND", "Product not found or access denied"));
                return;
            }
            Json::Value msg;
            msg["message"] = "Product deactivated successfully";
            callback(200, Response::success(msg));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to deactivate product"));
        },
        productId, sellerId
    );
}

void SellerService::getOrders(const std::string &sellerId,
                               std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT DISTINCT o.id, o.total, o.status, o.user_id, "
        "to_char(o.created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
        "to_char(o.updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at, "
        "u.username as customer_name "
        "FROM orders o "
        "JOIN order_items oi ON o.id = oi.order_id "
        "JOIN users u ON o.user_id = u.id "
        "WHERE oi.seller_id = $1 "
        "ORDER BY o.created_at DESC",
        [callback, dbClient, sellerId](const orm::Result &result) {
            Json::Value orders(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value order;
                order["id"] = row["id"].as<std::string>();
                order["total"] = row["total"].as<double>();
                order["status"] = row["status"].as<std::string>();
                order["customer_name"] = row["customer_name"].as<std::string>();
                order["created_at"] = row["created_at"].as<std::string>();
                order["updated_at"] = row["updated_at"].as<std::string>();
                orders.append(order);
            }
            callback(200, Response::success(orders));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to fetch orders"));
        },
        sellerId
    );
}

void SellerService::updateOrderStatus(const std::string &sellerId, const std::string &orderId,
                                       const std::string &newStatus,
                                       std::function<void(int, Json::Value)> callback) {
    if (newStatus.empty()) {
        callback(400, Response::error("VALIDATION_ERROR", "Status is required"));
        return;
    }

    // Check if new status is valid
    bool validStatus = false;
    for (const auto &pair : validTransitions) {
        if (pair.first == newStatus) {
            validStatus = true;
            break;
        }
        for (const auto &s : pair.second) {
            if (s == newStatus) {
                validStatus = true;
                break;
            }
        }
        if (validStatus) break;
    }

    if (!validStatus) {
        callback(400, Response::error("INVALID_STATUS", "Invalid order status"));
        return;
    }

    auto dbClient = app().getDbClient();

    // Verify seller has items in this order and get current status
    dbClient->execSqlAsync(
        "SELECT DISTINCT o.status FROM orders o "
        "JOIN order_items oi ON o.id = oi.order_id "
        "WHERE o.id = $1 AND oi.seller_id = $2",
        [dbClient, orderId, newStatus, callback](const orm::Result &result) {
            if (result.empty()) {
                callback(404, Response::error("ORDER_NOT_FOUND", "Order not found or access denied"));
                return;
            }

            std::string currentStatus = result[0]["status"].as<std::string>();

            // Validate transition
            auto it = validTransitions.find(currentStatus);
            if (it == validTransitions.end() || it->second.find(newStatus) == it->second.end()) {
                callback(400, Response::error("INVALID_TRANSITION",
                    "Cannot change status from " + currentStatus + " to " + newStatus));
                return;
            }

            dbClient->execSqlAsync(
                "UPDATE orders SET status = $2, updated_at = NOW() "
                "WHERE id = $1 "
                "RETURNING id, status, to_char(updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at",
                [callback](const orm::Result &updateResult) {
                    Json::Value order;
                    order["id"] = updateResult[0]["id"].as<std::string>();
                    order["status"] = updateResult[0]["status"].as<std::string>();
                    order["updated_at"] = updateResult[0]["updated_at"].as<std::string>();
                    order["message"] = "Order status updated";
                    callback(200, Response::success(order));
                },
                [callback](const orm::DrogonDbException &e) {
                    callback(500, Response::error("SERVER_ERROR", "Failed to update status"));
                },
                orderId, newStatus
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Database error"));
        },
        orderId, sellerId
    );
}
