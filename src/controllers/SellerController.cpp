#include "controllers/SellerController.h"
#include "services/SellerService.h"
#include "middleware/AuthMiddleware.h"
#include "utils/Response.h"
#include "utils/Validator.h"

void SellerController::getDashboard(const HttpRequestPtr &req,
                                     std::function<void(const HttpResponsePtr &)> &&callback) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    AuthMiddleware::requireSeller(req, *cb, [cb](std::string userId) {
        SellerService::getDashboard(userId, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}

void SellerController::getProducts(const HttpRequestPtr &req,
                                    std::function<void(const HttpResponsePtr &)> &&callback) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    AuthMiddleware::requireSeller(req, *cb, [cb](std::string userId) {
        SellerService::getProducts(userId, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}

void SellerController::createProduct(const HttpRequestPtr &req,
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
    AuthMiddleware::requireSeller(req, *cb, [cb, json](std::string userId) {
        SellerService::createProduct(userId, *json, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}

void SellerController::updateProduct(const HttpRequestPtr &req,
                                      std::function<void(const HttpResponsePtr &)> &&callback,
                                      const std::string &id) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));

    if (!Validator::isValidUUID(id)) {
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

    auto json = std::make_shared<Json::Value>(*jsonPtr);
    std::string productId = id;
    AuthMiddleware::requireSeller(req, *cb, [cb, json, productId](std::string userId) {
        SellerService::updateProduct(userId, productId, *json, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}

void SellerController::deleteProduct(const HttpRequestPtr &req,
                                      std::function<void(const HttpResponsePtr &)> &&callback,
                                      const std::string &id) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));

    if (!Validator::isValidUUID(id)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Invalid product ID"));
        resp->setStatusCode(k400BadRequest);
        (*cb)(resp);
        return;
    }

    std::string productId = id;
    AuthMiddleware::requireSeller(req, *cb, [cb, productId](std::string userId) {
        SellerService::deleteProduct(userId, productId, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}

void SellerController::getOrders(const HttpRequestPtr &req,
                                  std::function<void(const HttpResponsePtr &)> &&callback) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    AuthMiddleware::requireSeller(req, *cb, [cb](std::string userId) {
        SellerService::getOrders(userId, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}

void SellerController::updateOrderStatus(const HttpRequestPtr &req,
                                          std::function<void(const HttpResponsePtr &)> &&callback,
                                          const std::string &id) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));

    if (!Validator::isValidUUID(id)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Invalid order ID"));
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

    std::string status = (*jsonPtr).get("status", "").asString();
    std::string orderId = id;
    AuthMiddleware::requireSeller(req, *cb, [cb, orderId, status](std::string userId) {
        SellerService::updateOrderStatus(userId, orderId, status, [cb](int st, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(st));
            (*cb)(resp);
        });
    });
}
