#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

class AuthController : public HttpController<AuthController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::registerUser, "/api/v1/auth/register", Post, Options);
    ADD_METHOD_TO(AuthController::loginUser, "/api/v1/auth/login", Post, Options);
    ADD_METHOD_TO(AuthController::logoutUser, "/api/v1/auth/logout", Post, Options);
    ADD_METHOD_TO(AuthController::getMe, "/api/v1/me", Get, Options);
    METHOD_LIST_END

    void registerUser(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
    void loginUser(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback);
    void logoutUser(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback);
    void getMe(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback);
};
