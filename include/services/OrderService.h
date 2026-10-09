#pragma once
#include <json/json.h>
#include <string>
#include <functional>

class OrderService {
public:
    static void createOrder(const std::string &userId,
                            std::function<void(int, Json::Value)> callback);
    static void getOrders(const std::string &userId,
                          std::function<void(int, Json::Value)> callback);
    static void getOrder(const std::string &userId, const std::string &orderId,
                         std::function<void(int, Json::Value)> callback);
};
