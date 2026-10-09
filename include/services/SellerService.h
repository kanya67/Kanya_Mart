#pragma once
#include <json/json.h>
#include <string>
#include <functional>

class SellerService {
public:
    static void getDashboard(const std::string &sellerId,
                             std::function<void(int, Json::Value)> callback);
    static void getProducts(const std::string &sellerId,
                            std::function<void(int, Json::Value)> callback);
    static void createProduct(const std::string &sellerId, const Json::Value &data,
                              std::function<void(int, Json::Value)> callback);
    static void updateProduct(const std::string &sellerId, const std::string &productId,
                              const Json::Value &data,
                              std::function<void(int, Json::Value)> callback);
    static void deleteProduct(const std::string &sellerId, const std::string &productId,
                              std::function<void(int, Json::Value)> callback);
    static void getOrders(const std::string &sellerId,
                          std::function<void(int, Json::Value)> callback);
    static void updateOrderStatus(const std::string &sellerId, const std::string &orderId,
                                  const std::string &status,
                                  std::function<void(int, Json::Value)> callback);
};
