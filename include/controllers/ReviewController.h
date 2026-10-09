#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

class ReviewController : public HttpController<ReviewController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ReviewController::createReview, "/api/v1/products/{product_id}/reviews", Post, Options);
    ADD_METHOD_TO(ReviewController::updateReview, "/api/v1/reviews/{id}", Put, Options);
    ADD_METHOD_TO(ReviewController::deleteReview, "/api/v1/reviews/{id}", Delete, Options);
    METHOD_LIST_END

    void createReview(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &product_id);
    void updateReview(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &id);
    void deleteReview(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &id);
};
