#include "controllers/ReviewController.h"
#include "services/ReviewService.h"
#include "middleware/AuthMiddleware.h"
#include "utils/Response.h"
#include "utils/Validator.h"

void ReviewController::createReview(const HttpRequestPtr &req,
                                     std::function<void(const HttpResponsePtr &)> &&callback,
                                     const std::string &product_id) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));

    if (!Validator::isValidUUID(product_id)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Invalid product ID"));
        resp->setStatusCode(k400BadRequest);
        (*cb)(resp);
        return;
    }

    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("INVALID_JSON", "Invalid JSON format"));
        resp->setStatusCode(k400BadRequest);
        (*cb)(resp);
        return;
    }

    int rating = (*jsonPtr).get("rating", 0).asInt();
    std::string comment = (*jsonPtr).get("comment", "").asString();

    if (!Validator::isValidRating(rating)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Rating must be between 1 and 5"));
        resp->setStatusCode(k400BadRequest);
        (*cb)(resp);
        return;
    }

    std::string pid = product_id;
    AuthMiddleware::requireAuth(req, *cb, [cb, pid, rating, comment](std::string userId, std::string role) {
        ReviewService::createReview(userId, pid, rating, comment,
            [cb](int status, Json::Value result) {
                auto resp = HttpResponse::newHttpJsonResponse(result);
                resp->setStatusCode(static_cast<HttpStatusCode>(status));
                (*cb)(resp);
            });
    });
}

void ReviewController::updateReview(const HttpRequestPtr &req,
                                     std::function<void(const HttpResponsePtr &)> &&callback,
                                     const std::string &id) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));

    if (!Validator::isValidUUID(id)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Invalid review ID"));
        resp->setStatusCode(k400BadRequest);
        (*cb)(resp);
        return;
    }

    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("INVALID_JSON", "Invalid JSON format"));
        resp->setStatusCode(k400BadRequest);
        (*cb)(resp);
        return;
    }

    int rating = (*jsonPtr).get("rating", 0).asInt();
    std::string comment = (*jsonPtr).get("comment", "").asString();

    if (!Validator::isValidRating(rating)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Rating must be between 1 and 5"));
        resp->setStatusCode(k400BadRequest);
        (*cb)(resp);
        return;
    }

    std::string reviewId = id;
    AuthMiddleware::requireAuth(req, *cb, [cb, reviewId, rating, comment](std::string userId, std::string role) {
        ReviewService::updateReview(userId, reviewId, rating, comment,
            [cb](int status, Json::Value result) {
                auto resp = HttpResponse::newHttpJsonResponse(result);
                resp->setStatusCode(static_cast<HttpStatusCode>(status));
                (*cb)(resp);
            });
    });
}

void ReviewController::deleteReview(const HttpRequestPtr &req,
                                     std::function<void(const HttpResponsePtr &)> &&callback,
                                     const std::string &id) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));

    if (!Validator::isValidUUID(id)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Invalid review ID"));
        resp->setStatusCode(k400BadRequest);
        (*cb)(resp);
        return;
    }

    std::string reviewId = id;
    AuthMiddleware::requireAuth(req, *cb, [cb, reviewId](std::string userId, std::string role) {
        ReviewService::deleteReview(userId, reviewId,
            [cb](int status, Json::Value result) {
                auto resp = HttpResponse::newHttpJsonResponse(result);
                resp->setStatusCode(static_cast<HttpStatusCode>(status));
                (*cb)(resp);
            });
    });
}
