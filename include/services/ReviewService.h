#pragma once
#include <json/json.h>
#include <string>
#include <functional>

class ReviewService {
public:
    static void createReview(const std::string &userId, const std::string &productId,
                             int rating, const std::string &comment,
                             std::function<void(int, Json::Value)> callback);
    static void updateReview(const std::string &userId, const std::string &reviewId,
                             int rating, const std::string &comment,
                             std::function<void(int, Json::Value)> callback);
    static void deleteReview(const std::string &userId, const std::string &reviewId,
                             std::function<void(int, Json::Value)> callback);
};
