#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

class CartController : public HttpController<CartController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CartController::getCart, "/api/v1/cart", Get, Options);
    ADD_METHOD_TO(CartController::addItem, "/api/v1/cart/items", Post, Options);
    ADD_METHOD_TO(CartController::updateItem, "/api/v1/cart/items/{product_id}", Put, Options);
    ADD_METHOD_TO(CartController::removeItem, "/api/v1/cart/items/{product_id}", Delete, Options);
    METHOD_LIST_END

    void getCart(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void addItem(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
    void updateItem(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    const std::string &product_id);
    void removeItem(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    const std::string &product_id);
};
