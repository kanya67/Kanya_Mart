#include "controllers/HealthController.h"
#include "utils/Response.h"

void HealthController::check(const HttpRequestPtr &req,
                              std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value data;
    data["status"] = "healthy";
    data["service"] = "KanyaMart";
    data["version"] = "1.0.0";
    auto resp = HttpResponse::newHttpJsonResponse(Response::success(data));
    callback(resp);
}
