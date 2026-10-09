#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

class OrderController : public HttpController<OrderController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(OrderController::createOrder, "/api/v1/orders", Post, Options);
    ADD_METHOD_TO(OrderController::getOrders, "/api/v1/orders", Get, Options);
    ADD_METHOD_TO(OrderController::getOrder, "/api/v1/orders/{id}", Get, Options);
    METHOD_LIST_END

    void createOrder(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);
    void getOrders(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback);
    void getOrder(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  const std::string &id);
};
