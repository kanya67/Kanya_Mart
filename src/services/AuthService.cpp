#include "services/AuthService.h"
#include "utils/Response.h"
#include <bcrypt.h>
#include <drogon/drogon.h>
#include <random>
#include <sstream>
#include <iomanip>

using namespace drogon;

std::string AuthService::generateSessionId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 64; i++) ss << dis(gen);
    return ss.str();
}

std::string AuthService::hashPassword(const std::string& password) {
    // Generate bcrypt hash with cost 12
    return bcrypt::generateHash(password, 12);
}

bool AuthService::verifyPassword(const std::string& password, const std::string& hash) {
    return bcrypt::validatePassword(password, hash);
}

void AuthService::registerUser(const Json::Value &userData,
                                std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    std::string username = userData["username"].asString();
    std::string email = userData["email"].asString();
    std::string password = userData["password"].asString();
    std::string role = userData.get("role", "customer").asString();

    // Check duplicate username
    dbClient->execSqlAsync(
        "SELECT id FROM users WHERE username = $1",
        [dbClient, username, email, password, role, callback](const orm::Result &result) {
            if (!result.empty()) {
                callback(409, Response::error("USERNAME_EXISTS", "Username already exists"));
                return;
            }
            // Check duplicate email
            dbClient->execSqlAsync(
                "SELECT id FROM users WHERE email = $1",
                [dbClient, username, email, password, role, callback](const orm::Result &result) {
                    if (!result.empty()) {
                        callback(409, Response::error("EMAIL_EXISTS", "Email already registered"));
                        return;
                    }
                    // Hash password and insert user
                    std::string passwordHash = AuthService::hashPassword(password);
                    dbClient->execSqlAsync(
                        "INSERT INTO users (username, email, password_hash, role) "
                        "VALUES ($1, $2, $3, $4) "
                        "RETURNING id, username, email, role, "
                        "to_char(created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
                        "to_char(updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at",
                        [callback](const orm::Result &result) {
                            if (result.empty()) {
                                callback(500, Response::error("SERVER_ERROR", "Failed to create user"));
                                return;
                            }
                            Json::Value user;
                            user["id"] = result[0]["id"].as<std::string>();
                            user["username"] = result[0]["username"].as<std::string>();
                            user["email"] = result[0]["email"].as<std::string>();
                            user["role"] = result[0]["role"].as<std::string>();
                            user["created_at"] = result[0]["created_at"].as<std::string>();
                            user["updated_at"] = result[0]["updated_at"].as<std::string>();
                            callback(201, Response::success(user));
                        },
                        [callback](const orm::DrogonDbException &e) {
                            callback(500, Response::error("SERVER_ERROR", "Database error during registration"));
                        },
                        username, email, passwordHash, role
                    );
                },
                [callback](const orm::DrogonDbException &e) {
                    callback(500, Response::error("SERVER_ERROR", "Database error"));
                },
                email
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Database error"));
        },
        username
    );
}

void AuthService::loginUser(const Json::Value &userData,
                             std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    std::string email = userData["email"].asString();
    std::string password = userData["password"].asString();

    dbClient->execSqlAsync(
        "SELECT id, username, email, password_hash, role, "
        "to_char(created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
        "to_char(updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at "
        "FROM users WHERE email = $1",
        [callback, password](const orm::Result &result) {
            if (result.empty()) {
                callback(401, Response::error("INVALID_CREDENTIALS", "Invalid email or password"));
                return;
            }
            std::string hash = result[0]["password_hash"].as<std::string>();
            if (!AuthService::verifyPassword(password, hash)) {
                callback(401, Response::error("INVALID_CREDENTIALS", "Invalid email or password"));
                return;
            }

            std::string userId = result[0]["id"].as<std::string>();
            std::string username = result[0]["username"].as<std::string>();
            std::string email = result[0]["email"].as<std::string>();
            std::string role = result[0]["role"].as<std::string>();
            std::string created = result[0]["created_at"].as<std::string>();
            std::string updated = result[0]["updated_at"].as<std::string>();

            // Create session
            AuthService::createSession(userId, [callback, userId, username, email, role, created, updated](std::string sessionId) {
                if (sessionId.empty()) {
                    callback(500, Response::error("SERVER_ERROR", "Failed to create session"));
                    return;
                }
                Json::Value user;
                user["id"] = userId;
                user["username"] = username;
                user["email"] = email;
                user["role"] = role;
                user["created_at"] = created;
                user["updated_at"] = updated;
                user["session_id"] = sessionId;
                callback(200, Response::success(user));
            });
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Database error"));
        },
        email
    );
}

void AuthService::createSession(const std::string &userId,
                                 std::function<void(std::string)> callback) {
    auto dbClient = app().getDbClient();
    std::string sessionId = generateSessionId();

    // Delete old sessions for this user first
    dbClient->execSqlAsync(
        "DELETE FROM sessions WHERE user_id = $1::uuid",
        [dbClient, sessionId, userId, callback](const orm::Result &) {
            dbClient->execSqlAsync(
                "INSERT INTO sessions (session_id, user_id, expires_at) "
                "VALUES ($1, $2::uuid, NOW() + INTERVAL '24 hours')",
                [sessionId, callback](const orm::Result &) {
                    callback(sessionId);
                },
                [callback](const orm::DrogonDbException &e) {
                    callback("");
                },
                sessionId, userId
            );
        },
        [callback](const orm::DrogonDbException &e) {
            callback("");
        },
        userId
    );
}

void AuthService::getUserBySessionId(const std::string &sessionId,
                                      std::function<void(int, Json::Value)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT u.id, u.username, u.email, u.role, "
        "to_char(u.created_at, 'YYYY-MM-DD HH24:MI:SS') as created_at, "
        "to_char(u.updated_at, 'YYYY-MM-DD HH24:MI:SS') as updated_at "
        "FROM sessions s JOIN users u ON s.user_id = u.id "
        "WHERE s.session_id = $1 AND s.expires_at > NOW()",
        [callback](const orm::Result &result) {
            if (result.empty()) {
                callback(401, Response::error("UNAUTHORIZED", "Session expired or invalid"));
                return;
            }
            Json::Value user;
            user["id"] = result[0]["id"].as<std::string>();
            user["username"] = result[0]["username"].as<std::string>();
            user["email"] = result[0]["email"].as<std::string>();
            user["role"] = result[0]["role"].as<std::string>();
            user["created_at"] = result[0]["created_at"].as<std::string>();
            user["updated_at"] = result[0]["updated_at"].as<std::string>();
            callback(200, Response::success(user));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(500, Response::error("SERVER_ERROR", "Database error"));
        },
        sessionId
    );
}

void AuthService::deleteSession(const std::string &sessionId,
                                 std::function<void(bool)> callback) {
    auto dbClient = app().getDbClient();
    dbClient->execSqlAsync(
        "DELETE FROM sessions WHERE session_id = $1",
        [callback](const orm::Result &) {
            callback(true);
        },
        [callback](const orm::DrogonDbException &e) {
            callback(false);
        },
        sessionId
    );
}
