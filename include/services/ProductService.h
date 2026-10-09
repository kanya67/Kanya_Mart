#pragma once
#include <json/json.h>
#include <string>
#include <functional>

class ProductService {
public:
    static void getProducts(const std::string &search, const std::string &category,
                            const std::string &sort, double minPrice, double maxPrice,
                            int minRating, int page, int perPage,
                            std::function<void(int, Json::Value)> callback);
    static void getProduct(const std::string &id,
                           std::function<void(int, Json::Value)> callback);
};
