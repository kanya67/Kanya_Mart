#pragma once
#include <nlohmann/json.hpp>

class AuthService {
public:
    static nlohmann::json registerUser(const nlohmann::json &userData);
};
