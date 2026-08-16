#include "services/AuthService.h"
#include "models/User.h"
#include <bcrypt.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <unordered_map>
#include <chrono>

using json = nlohmann::json;

extern std::unordered_map<std::string, User> users;
extern std::mutex users_mutex;

std::string generateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);
    
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4";
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen);
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    return ss.str();
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    char buf[100] = {0};
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));
    return std::string(buf);
}

json AuthService::registerUser(const json &userData) {
    if (!userData.contains("username") || !userData.contains("email") || !userData.contains("password")) {
        return {{"error", "Username, email, and password are required"}};
    }
    
    std::string username = userData["username"];
    std::string email = userData["email"];
    std::string password = userData["password"];
    
    if (username.empty() || email.empty() || password.empty()) {
        return {{"error", "Username, email, and password are required"}};
    }
    
    std::lock_guard<std::mutex> lock(users_mutex);
    
    // Check duplicates
    if (users.find(username) != users.end()) {
        return {{"error", "Username already exists"}};
    }
    for (const auto& pair : users) {
        if (pair.second.email == email) {
            return {{"error", "Email already registered"}};
        }
    }
    
    // Hash password
    char salt[BCRYPT_HASHSIZE];
    char hash[BCRYPT_HASHSIZE];
    bcrypt_gensalt(12, salt);
    bcrypt_hashpw(password.c_str(), salt, hash);
    
    // Create User
    User newUser;
    newUser.id = generateUUID();
    newUser.username = username;
    newUser.email = email;
    newUser.password_hash = hash;
    newUser.role = "customer";
    newUser.created_at = getCurrentTimestamp();
    newUser.updated_at = newUser.created_at;
    
    users[username] = newUser;
    
    // Return public response
    return {
        {"id", newUser.id},
        {"username", newUser.username},
        {"email", newUser.email},
        {"role", newUser.role},
        {"created_at", newUser.created_at},
        {"updated_at", newUser.updated_at}
    };
}
