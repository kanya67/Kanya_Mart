#pragma once
#include <json/json.h>
#include <string>
#include <functional>

class CartService {
public:
    static void getCart(const std::string &userId,
                        std::function<void(int, Json::Value)> callback);
    static void addItem(const std::string &userId, const std::string &productId, int quantity,
                        std::function<void(int, Json::Value)> callback);
    static void updateItem(const std::string &userId, const std::string &productId, int quantity,
                           std::function<void(int, Json::Value)> callback);
    static void removeItem(const std::string &userId, const std::string &productId,
                           std::function<void(int, Json::Value)> callback);
};
