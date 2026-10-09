#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

class SellerController : public HttpController<SellerController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SellerController::getDashboard, "/api/v1/seller/dashboard", Get, Options);
    ADD_METHOD_TO(SellerController::getProducts, "/api/v1/seller/products", Get, Options);
    ADD_METHOD_TO(SellerController::createProduct, "/api/v1/seller/products", Post, Options);
    ADD_METHOD_TO(SellerController::updateProduct, "/api/v1/seller/products/{id}", Put, Options);
    ADD_METHOD_TO(SellerController::deleteProduct, "/api/v1/seller/products/{id}", Delete, Options);
    ADD_METHOD_TO(SellerController::getOrders, "/api/v1/seller/orders", Get, Options);
    ADD_METHOD_TO(SellerController::updateOrderStatus, "/api/v1/seller/orders/{id}/status", Put, Options);
    METHOD_LIST_END

    void getDashboard(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
    void getProducts(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);
    void createProduct(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback);
    void updateProduct(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback,
                       const std::string &id);
    void deleteProduct(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback,
                       const std::string &id);
    void getOrders(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback);
    void updateOrderStatus(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback,
                           const std::string &id);
};
