#pragma once
#include <drogon/HttpRequest.h>
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include <string>
#include <functional>

namespace AuthMiddleware {

// Extract session ID from cookie
inline std::string getSessionId(const drogon::HttpRequestPtr &req) {
    auto cookie = req->getCookie("kanyamart_session");
    return cookie;
}

// Authenticate user from session cookie and call callback with userId and role
inline void authenticate(const drogon::HttpRequestPtr &req,
                         std::function<void(bool, std::string, std::string)> callback) {
    auto sessionId = getSessionId(req);
    if (sessionId.empty()) {
        callback(false, "", "");
        return;
    }

    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT s.user_id, u.role FROM sessions s "
        "JOIN users u ON s.user_id = u.id "
        "WHERE s.session_id = $1 AND s.expires_at > NOW()",
        [callback](const drogon::orm::Result &result) {
            if (result.empty()) {
                callback(false, "", "");
            } else {
                callback(true,
                         result[0]["user_id"].as<std::string>(),
                         result[0]["role"].as<std::string>());
            }
        },
        [callback](const drogon::orm::DrogonDbException &e) {
            callback(false, "", "");
        },
        sessionId
    );
}

// Require authentication - sends 401 if not authenticated
inline void requireAuth(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &httpCallback,
                        std::function<void(std::string userId, std::string role)> onSuccess) {
    auto cb = httpCallback; // copy for lambda
    authenticate(req, [cb, onSuccess](bool ok, std::string userId, std::string role) {
        if (!ok) {
            Json::Value err;
            err["success"] = false;
            err["error"]["code"] = "UNAUTHORIZED";
            err["error"]["message"] = "Authentication required";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k401Unauthorized);
            cb(resp);
            return;
        }
        onSuccess(userId, role);
    });
}

// Require seller role
inline void requireSeller(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &httpCallback,
                          std::function<void(std::string userId)> onSuccess) {
    auto cb = httpCallback;
    requireAuth(req, httpCallback, [cb, onSuccess](std::string userId, std::string role) {
        if (role != "seller" && role != "admin") {
            Json::Value err;
            err["success"] = false;
            err["error"]["code"] = "FORBIDDEN";
            err["error"]["message"] = "Seller access required";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k403Forbidden);
            cb(resp);
            return;
        }
        onSuccess(userId);
    });
}

} // namespace AuthMiddleware
