#include "controllers/CartController.h"
#include "services/CartService.h"
#include "middleware/AuthMiddleware.h"
#include "utils/Response.h"
#include "utils/Validator.h"

void CartController::getCart(const HttpRequestPtr &req,
                              std::function<void(const HttpResponsePtr &)> &&callback) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    AuthMiddleware::requireAuth(req, *cb, [cb](std::string userId, std::string role) {
        CartService::getCart(userId, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}

void CartController::addItem(const HttpRequestPtr &req,
                              std::function<void(const HttpResponsePtr &)> &&callback) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("INVALID_JSON", "Invalid JSON format"));
        resp->setStatusCode(k400BadRequest);
        (*cb)(resp);
        return;
    }

    auto json = std::make_shared<Json::Value>(*jsonPtr);
    AuthMiddleware::requireAuth(req, *cb, [cb, json](std::string userId, std::string role) {
        std::string productId = (*json).get("product_id", "").asString();
        int quantity = (*json).get("quantity", 1).asInt();

        if (!Validator::isValidUUID(productId)) {
            auto resp = HttpResponse::newHttpJsonResponse(
                Response::error("VALIDATION_ERROR", "Invalid product ID"));
            resp->setStatusCode(drogon::k400BadRequest);
            (*cb)(resp);
            return;
        }

        if (quantity < 1) {
            auto resp = HttpResponse::newHttpJsonResponse(
                Response::error("VALIDATION_ERROR", "Quantity must be at least 1"));
            resp->setStatusCode(drogon::k400BadRequest);
            (*cb)(resp);
            return;
        }

        CartService::addItem(userId, productId, quantity, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}

void CartController::updateItem(const HttpRequestPtr &req,
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

    int quantity = (*jsonPtr).get("quantity", 0).asInt();
    std::string pid = product_id;

    AuthMiddleware::requireAuth(req, *cb, [cb, pid, quantity](std::string userId, std::string role) {
        if (quantity <= 0) {
            // Remove item if quantity is 0 or less
            CartService::removeItem(userId, pid, [cb](int status, Json::Value result) {
                auto resp = HttpResponse::newHttpJsonResponse(result);
                resp->setStatusCode(static_cast<HttpStatusCode>(status));
                (*cb)(resp);
            });
        } else {
            CartService::updateItem(userId, pid, quantity, [cb](int status, Json::Value result) {
                auto resp = HttpResponse::newHttpJsonResponse(result);
                resp->setStatusCode(static_cast<HttpStatusCode>(status));
                (*cb)(resp);
            });
        }
    });
}

void CartController::removeItem(const HttpRequestPtr &req,
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

    std::string pid = product_id;
    AuthMiddleware::requireAuth(req, *cb, [cb, pid](std::string userId, std::string role) {
        CartService::removeItem(userId, pid, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}
