#pragma once
#include <string>

struct User {
    std::string id;
    std::string username;
    std::string email;
    std::string password_hash;
    std::string role;
    std::string created_at;
    std::string updated_at;
};
