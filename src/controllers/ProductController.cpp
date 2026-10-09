#include "controllers/ProductController.h"
#include "services/ProductService.h"
#include "utils/Response.h"
#include "utils/Validator.h"

void ProductController::getProducts(const HttpRequestPtr &req,
                                     std::function<void(const HttpResponsePtr &)> &&callback) {
    std::string search = req->getParameter("search");
    std::string category = req->getParameter("category");
    std::string sort = req->getParameter("sort");
    std::string minPriceStr = req->getParameter("min_price");
    std::string maxPriceStr = req->getParameter("max_price");
    std::string minRatingStr = req->getParameter("min_rating");
    std::string pageStr = req->getParameter("page");
    std::string perPageStr = req->getParameter("per_page");

    double minPrice = 0, maxPrice = 10000000;
    int minRating = 0, page = 1, perPage = 20;

    try {
        if (!minPriceStr.empty()) minPrice = std::stod(minPriceStr);
        if (!maxPriceStr.empty()) maxPrice = std::stod(maxPriceStr);
        if (!minRatingStr.empty()) minRating = std::stoi(minRatingStr);
        if (!pageStr.empty()) page = std::stoi(pageStr);
        if (!perPageStr.empty()) perPage = std::stoi(perPageStr);
    } catch (...) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Invalid filter parameters"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    if (page < 1) page = 1;
    if (perPage < 1 || perPage > 100) perPage = 20;

    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    ProductService::getProducts(search, category, sort, minPrice, maxPrice, minRating,
                                page, perPage,
                                [cb](int status, Json::Value result) {
        auto resp = HttpResponse::newHttpJsonResponse(result);
        resp->setStatusCode(static_cast<HttpStatusCode>(status));
        (*cb)(resp);
    });
}

void ProductController::getProduct(const HttpRequestPtr &req,
                                    std::function<void(const HttpResponsePtr &)> &&callback,
                                    const std::string &id) {
    if (!Validator::isValidUUID(id)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Invalid product ID format"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    ProductService::getProduct(id, [cb](int status, Json::Value result) {
        auto resp = HttpResponse::newHttpJsonResponse(result);
        resp->setStatusCode(static_cast<HttpStatusCode>(status));
        (*cb)(resp);
    });
}
