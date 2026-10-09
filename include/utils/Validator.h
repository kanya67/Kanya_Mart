#pragma once
#include <string>
#include <regex>
#include <algorithm>

namespace Validator {

inline bool isValidEmail(const std::string &email) {
    static const std::regex emailRegex(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
    );
    return std::regex_match(email, emailRegex);
}

inline bool isValidUUID(const std::string &uuid) {
    static const std::regex uuidRegex(
        R"(^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$)"
    );
    return std::regex_match(uuid, uuidRegex);
}

inline bool isValidPassword(const std::string &password) {
    return password.length() >= 6;
}

inline bool isValidUsername(const std::string &username) {
    if (username.length() < 3 || username.length() > 50) return false;
    static const std::regex usernameRegex(R"(^[a-zA-Z0-9_]+$)");
    return std::regex_match(username, usernameRegex);
}

inline bool isValidPrice(double price) {
    return price > 0 && price < 10000000;
}

inline bool isValidQuantity(int qty) {
    return qty > 0 && qty <= 10000;
}

inline bool isValidRating(int rating) {
    return rating >= 1 && rating <= 5;
}

inline bool isValidCategory(const std::string &cat) {
    static const std::vector<std::string> valid = {
        "Electronics", "Fashion", "Home", "Beauty",
        "Sports", "Accessories", "Books", "Gaming"
    };
    return std::find(valid.begin(), valid.end(), cat) != valid.end();
}

inline std::string trim(const std::string &str) {
    auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace Validator
