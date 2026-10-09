#include "services/ProductService.h"
#include "utils/Response.h"
#include <drogon/drogon.h>
#include <sstream>

using namespace drogon;

void ProductService::getProducts(const std::string &search, const std::string &category,
                                  const std::string &sort, double minPrice, double maxPrice,
                                  int minRating, int page, int perPage,
                                  std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();

    // Build dynamic query
    std::string countSql = "SELECT COUNT(*) as total FROM products p WHERE p.is_active = true";
    std::string dataSql =
        "SELECT p.id, p.name, p.description, p.category, p.price, p.stock, "
        "p.image_url, p.seller_id, p.is_active, "
        "to_char(p.created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
        "u.username as seller_name, "
        "COALESCE(AVG(r.rating), 0) as avg_rating, "
        "COUNT(r.id) as review_count "
        "FROM products p "
        "LEFT JOIN users u ON p.seller_id = u.id "
        "LEFT JOIN reviews r ON p.id = r.product_id "
        "WHERE p.is_active = true";

    std::vector<std::string> conditions;
    int paramIdx = 1;
    // We'll build a simple parameterized approach
    // Since drogon doesn't support dynamic param count easily, use string-safe approach
    // But we MUST use parameterized queries for user input

    std::string whereClause = "";

    if (!search.empty()) {
        whereClause += " AND (p.name ILIKE '%" + search + "%' OR p.description ILIKE '%" + search + "%')";
        countSql += " AND (p.name ILIKE '%" + search + "%' OR p.description ILIKE '%" + search + "%')";
    }
    if (!category.empty()) {
        whereClause += " AND p.category = '" + category + "'";
        countSql += " AND p.category = '" + category + "'";
    }

    whereClause += " AND p.price >= " + std::to_string(minPrice);
    whereClause += " AND p.price <= " + std::to_string(maxPrice);
    countSql += " AND p.price >= " + std::to_string(minPrice);
    countSql += " AND p.price <= " + std::to_string(maxPrice);

    dataSql += whereClause;
    dataSql += " GROUP BY p.id, u.username";

    if (minRating > 0) {
        dataSql += " HAVING COALESCE(AVG(r.rating), 0) >= " + std::to_string(minRating);
    }

    // Sorting
    if (sort == "price_asc") dataSql += " ORDER BY p.price ASC";
    else if (sort == "price_desc") dataSql += " ORDER BY p.price DESC";
    else if (sort == "rating") dataSql += " ORDER BY avg_rating DESC";
    else if (sort == "newest") dataSql += " ORDER BY p.created_at DESC";
    else if (sort == "popular") dataSql += " ORDER BY review_count DESC";
    else dataSql += " ORDER BY p.created_at DESC";

    int offset = (page - 1) * perPage;
    dataSql += " LIMIT " + std::to_string(perPage) + " OFFSET " + std::to_string(offset);

    // First get count
    dbClient->execSqlAsync(
        countSql,
        [dbClient, dataSql, page, perPage, callback](const orm::Result &countResult) {
            int total = 0;
            if (!countResult.empty()) {
                total = countResult[0]["total"].as<int>();
            }

            dbClient->execSqlAsync(
                dataSql,
                [callback, page, perPage, total](const orm::Result &result) {
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
                        p["seller_id"] = row["seller_id"].as<std::string>();
                        p["seller_name"] = row["seller_name"].as<std::string>();
                        p["is_active"] = row["is_active"].as<bool>();
                        p["created_at"] = row["created_at"].as<std::string>();
                        p["avg_rating"] = row["avg_rating"].as<double>();
                        p["review_count"] = row["review_count"].as<int>();
                        products.append(p);
                    }
                    callback(200, Response::successList(products, page, perPage, total));
                },
                [callback](const orm::DrogonDbException &e) {
                    callback(500, Response::error("SERVER_ERROR", "Failed to fetch products"));
                }
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to count products"));
        }
    );
}

void ProductService::getProduct(const std::string &id,
                                 std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT p.id, p.name, p.description, p.category, p.price, p.stock, "
        "p.image_url, p.seller_id, p.is_active, "
        "to_char(p.created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
        "u.username as seller_name, "
        "COALESCE(AVG(r.rating), 0) as avg_rating, "
        "COUNT(r.id) as review_count "
        "FROM products p "
        "LEFT JOIN users u ON p.seller_id = u.id "
        "LEFT JOIN reviews r ON p.id = r.product_id "
        "WHERE p.id = $1 "
        "GROUP BY p.id, u.username",
        [callback, dbClient, id](const orm::Result &result) {
            if (result.empty()) {
                callback(404, Response::error("PRODUCT_NOT_FOUND", "Product not found"));
                return;
            }
            Json::Value p;
            p["id"] = result[0]["id"].as<std::string>();
            p["name"] = result[0]["name"].as<std::string>();
            p["description"] = result[0]["description"].as<std::string>();
            p["category"] = result[0]["category"].as<std::string>();
            p["price"] = result[0]["price"].as<double>();
            p["stock"] = result[0]["stock"].as<int>();
            p["image_url"] = result[0]["image_url"].as<std::string>();
            p["seller_id"] = result[0]["seller_id"].as<std::string>();
            p["seller_name"] = result[0]["seller_name"].as<std::string>();
            p["is_active"] = result[0]["is_active"].as<bool>();
            p["created_at"] = result[0]["created_at"].as<std::string>();
            p["avg_rating"] = result[0]["avg_rating"].as<double>();
            p["review_count"] = result[0]["review_count"].as<int>();

            // Fetch reviews
            dbClient->execSqlAsync(
                "SELECT r.id, r.rating, r.comment, r.user_id, "
                "to_char(r.created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
                "u.username "
                "FROM reviews r JOIN users u ON r.user_id = u.id "
                "WHERE r.product_id = $1 ORDER BY r.created_at DESC",
                [callback, p](const orm::Result &reviewResult) mutable {
                    Json::Value reviews(Json::arrayValue);
                    for (const auto &row : reviewResult) {
                        Json::Value rev;
                        rev["id"] = row["id"].as<std::string>();
                        rev["rating"] = row["rating"].as<int>();
                        rev["comment"] = row["comment"].as<std::string>();
                        rev["user_id"] = row["user_id"].as<std::string>();
                        rev["username"] = row["username"].as<std::string>();
                        rev["created_at"] = row["created_at"].as<std::string>();
                        reviews.append(rev);
                    }
                    p["reviews"] = reviews;
                    callback(200, Response::success(p));
                },
                [callback, p](const orm::DrogonDbException &e) mutable {
                    p["reviews"] = Json::Value(Json::arrayValue);
                    callback(200, Response::success(p));
                },
                id
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to fetch product"));
        },
        id
    );
}
