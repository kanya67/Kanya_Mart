#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

class HealthController : public HttpController<HealthController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(HealthController::check, "/api/v1/health", Get, Options);
    METHOD_LIST_END

    void check(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback);
};
