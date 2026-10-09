#pragma once
#include <json/json.h>
#include <string>
#include <functional>
#include <drogon/orm/DbClient.h>

class AuthService {
public:
    static void registerUser(const Json::Value &userData,
                             std::function<void(int, Json::Value)> callback);
    static void loginUser(const Json::Value &userData,
                          std::function<void(int, Json::Value)> callback);
    static void getUserBySessionId(const std::string &sessionId,
                                   std::function<void(int, Json::Value)> callback);
    static void createSession(const std::string &userId,
                              std::function<void(std::string)> callback);
    static void deleteSession(const std::string &sessionId,
                              std::function<void(bool)> callback);
    static std::string generateSessionId();
    static std::string hashPassword(const std::string &password);
    static bool verifyPassword(const std::string &password, const std::string &hash);
};
