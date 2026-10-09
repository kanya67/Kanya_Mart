#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

class ProductController : public HttpController<ProductController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ProductController::getProducts, "/api/v1/products", Get, Options);
    ADD_METHOD_TO(ProductController::getProduct, "/api/v1/products/{id}", Get, Options);
    METHOD_LIST_END

    void getProducts(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);
    void getProduct(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    const std::string &id);
};
