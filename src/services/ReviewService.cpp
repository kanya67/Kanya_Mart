#include "services/ReviewService.h"
#include "utils/Response.h"
#include <drogon/drogon.h>

using namespace drogon;

void ReviewService::createReview(const std::string &userId, const std::string &productId,
                                  int rating, const std::string &comment,
                                  std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();

    // Verify product exists
    dbClient->execSqlAsync(
        "SELECT id FROM products WHERE id = $1",
        [dbClient, userId, productId, rating, comment, callback](const orm::Result &result) {
            if (result.empty()) {
                callback(404, Response::error("PRODUCT_NOT_FOUND", "Product not found"));
                return;
            }

            // Verify user has purchased this product (delivered order)
            dbClient->execSqlAsync(
                "SELECT oi.id FROM order_items oi "
                "JOIN orders o ON oi.order_id = o.id "
                "WHERE o.user_id = $1 AND oi.product_id = $2 AND o.status = 'DELIVERED' "
                "LIMIT 1",
                [dbClient, userId, productId, rating, comment, callback](const orm::Result &purchaseResult) {
                    if (purchaseResult.empty()) {
                        callback(403, Response::error("NOT_PURCHASED",
                            "You can only review products you have purchased and received"));
                        return;
                    }

                    // Check for existing review
                    dbClient->execSqlAsync(
                        "SELECT id FROM reviews WHERE user_id = $1 AND product_id = $2",
                        [dbClient, userId, productId, rating, comment, callback](const orm::Result &existResult) {
                            if (!existResult.empty()) {
                                callback(409, Response::error("DUPLICATE_REVIEW",
                                    "You have already reviewed this product. Use PUT to update."));
                                return;
                            }

                            // Create review
                            dbClient->execSqlAsync(
                                "INSERT INTO reviews (user_id, product_id, rating, comment) "
                                "VALUES ($1, $2, $3, $4) "
                                "RETURNING id, rating, comment, "
                                "to_char(created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at",
                                [callback](const orm::Result &reviewResult) {
                                    Json::Value review;
                                    review["id"] = reviewResult[0]["id"].as<std::string>();
                                    review["rating"] = reviewResult[0]["rating"].as<int>();
                                    review["comment"] = reviewResult[0]["comment"].as<std::string>();
                                    review["created_at"] = reviewResult[0]["created_at"].as<std::string>();
                                    review["message"] = "Review submitted successfully";
                                    callback(201, Response::success(review));
                                },
                                [callback](const orm::DrogonDbException &e) {
                                    callback(500, Response::error("SERVER_ERROR", "Failed to create review"));
                                },
                                userId, productId, rating, comment
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
                userId, productId
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Database error"));
        },
        productId
    );
}

void ReviewService::updateReview(const std::string &userId, const std::string &reviewId,
                                  int rating, const std::string &comment,
                                  std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "UPDATE reviews SET rating = $3, comment = $4, updated_at = NOW() "
        "WHERE id = $1 AND user_id = $2 "
        "RETURNING id, rating, comment, "
        "to_char(updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at",
        [callback](const orm::Result &result) {
            if (result.empty()) {
                callback(404, Response::error("REVIEW_NOT_FOUND", "Review not found or access denied"));
                return;
            }
            Json::Value review;
            review["id"] = result[0]["id"].as<std::string>();
            review["rating"] = result[0]["rating"].as<int>();
            review["comment"] = result[0]["comment"].as<std::string>();
            review["updated_at"] = result[0]["updated_at"].as<std::string>();
            review["message"] = "Review updated successfully";
            callback(200, Response::success(review));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to update review"));
        },
        reviewId, userId, rating, comment
    );
}

void ReviewService::deleteReview(const std::string &userId, const std::string &reviewId,
                                  std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "DELETE FROM reviews WHERE id = $1 AND user_id = $2 RETURNING id",
        [callback](const orm::Result &result) {
            if (result.empty()) {
                callback(404, Response::error("REVIEW_NOT_FOUND", "Review not found or access denied"));
                return;
            }
            Json::Value msg;
            msg["message"] = "Review deleted successfully";
            callback(200, Response::success(msg));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Failed to delete review"));
        },
        reviewId, userId
    );
}
