#include "controllers/AuthController.h"
#include "services/AuthService.h"
#include "utils/Response.h"
#include "utils/Validator.h"

void AuthController::registerUser(const HttpRequestPtr &req,
                                  std::function<void(const HttpResponsePtr &)> &&callback) {
    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("INVALID_JSON", "Invalid JSON format"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    const auto &json = *jsonPtr;
    std::string username = json.get("username", "").asString();
    std::string email = json.get("email", "").asString();
    std::string password = json.get("password", "").asString();
    std::string role = json.get("role", "customer").asString();

    // Validate inputs
    if (username.empty() || email.empty() || password.empty()) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Username, email, and password are required"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    if (!Validator::isValidUsername(username)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Username must be 3-50 characters, alphanumeric and underscores only"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    if (!Validator::isValidEmail(email)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Invalid email format"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    if (!Validator::isValidPassword(password)) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Password must be at least 6 characters"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    // Only allow customer or seller registration
    if (role != "customer" && role != "seller") {
        role = "customer";
    }

    Json::Value userData;
    userData["username"] = username;
    userData["email"] = email;
    userData["password"] = password;
    userData["role"] = role;

    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    AuthService::registerUser(userData, [cb](int status, Json::Value result) {
        auto resp = HttpResponse::newHttpJsonResponse(result);
        resp->setStatusCode(static_cast<HttpStatusCode>(status));
        (*cb)(resp);
    });
}

void AuthController::loginUser(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback) {
    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("INVALID_JSON", "Invalid JSON format"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    const auto &json = *jsonPtr;
    std::string email = json.get("email", "").asString();
    std::string password = json.get("password", "").asString();

    if (email.empty() || password.empty()) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("VALIDATION_ERROR", "Email and password are required"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    AuthService::loginUser(*jsonPtr, [cb](int status, Json::Value result) {
        auto resp = HttpResponse::newHttpJsonResponse(result);
        resp->setStatusCode(static_cast<HttpStatusCode>(status));
        // Set session cookie if login succeeded
        if (status == 200 && result.isMember("data") && result["data"].isMember("session_id")) {
            auto cookie = Cookie("kanyamart_session", result["data"]["session_id"].asString());
            cookie.setHttpOnly(true);
            cookie.setPath("/");
            cookie.setMaxAge(86400); // 24 hours
            resp->addCookie(cookie);
            // Remove session_id from response body
            result["data"].removeMember("session_id");
            resp = HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(static_cast<HttpStatusCode>(status));
            resp->addCookie(cookie);
        }
        (*cb)(resp);
    });
}

void AuthController::logoutUser(const HttpRequestPtr &req,
                                 std::function<void(const HttpResponsePtr &)> &&callback) {
    auto sessionId = req->getCookie("kanyamart_session");
    if (sessionId.empty()) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::success(Json::Value("Logged out")));
        callback(resp);
        return;
    }

    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    AuthService::deleteSession(sessionId, [cb](bool ok) {
        Json::Value msg;
        msg = "Logged out successfully";
        auto resp = HttpResponse::newHttpJsonResponse(Response::success(msg));
        // Clear cookie
        auto cookie = Cookie("kanyamart_session", "");
        cookie.setHttpOnly(true);
        cookie.setPath("/");
        cookie.setMaxAge(0);
        resp->addCookie(cookie);
        (*cb)(resp);
    });
}

void AuthController::getMe(const HttpRequestPtr &req,
                            std::function<void(const HttpResponsePtr &)> &&callback) {
    auto sessionId = req->getCookie("kanyamart_session");
    if (sessionId.empty()) {
        auto resp = HttpResponse::newHttpJsonResponse(
            Response::error("UNAUTHORIZED", "Authentication required"));
        resp->setStatusCode(k401Unauthorized);
        callback(resp);
        return;
    }

    auto cb = std::make_shared<std::function<void(const HttpResponsePtr &)>>(std::move(callback));
    AuthService::getUserBySessionId(sessionId, [cb](int status, Json::Value result) {
        auto resp = HttpResponse::newHttpJsonResponse(result);
        resp->setStatusCode(static_cast<HttpStatusCode>(status));
        (*cb)(resp);
    });
}
