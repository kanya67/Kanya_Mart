#pragma once
#include <string>
#include <json/json.h>

struct User {
    std::string id;
    std::string username;
    std::string email;
    std::string password_hash;
    std::string role;
    std::string created_at;
    std::string updated_at;

    Json::Value toJson() const {
        Json::Value json;
        json["id"] = id;
        json["username"] = username;
        json["email"] = email;
        json["role"] = role;
        json["created_at"] = created_at;
        json["updated_at"] = updated_at;
        // Never include password_hash
        return json;
    }
};
