#include "controllers/OrderController.h"
#include "services/OrderService.h"
#include "middleware/AuthMiddleware.h"
#include "utils/Response.h"
#include "utils/Validator.h"

void OrderController::createOrder(const HttpRequestPtr &req,
                                   std::function<void(const HttpResponsePtr &)> &&callback) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    AuthMiddleware::requireAuth(req, *cb, [cb](std::string userId, std::string role) {
        OrderService::createOrder(userId, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}

void OrderController::getOrders(const HttpRequestPtr &req,
                                 std::function<void(const HttpResponsePtr &)> &&callback) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    AuthMiddleware::requireAuth(req, *cb, [cb](std::string userId, std::string role) {
        OrderService::getOrders(userId, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}

void OrderController::getOrder(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback,
                                const std::string &id) {
    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));

    if (!Validator::isValidUUID(id)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Invalid order ID format"));
        resp->setStatusCode(k400BadRequest);
        (*cb)(resp);
        return;
    }

    std::string orderId = id;
    AuthMiddleware::requireAuth(req, *cb, [cb, orderId](std::string userId, std::string role) {
        OrderService::getOrder(userId, orderId, [cb](int status, Json::Value result) {
            auto resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            (*cb)(resp);
        });
    });
}
