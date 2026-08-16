#include "controllers/AuthController.h"
#include "services/AuthService.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void AuthController::initRoutes(httplib::Server &svr) {
    svr.Post("/api/v1/auth/register", AuthController::registerUser);
}

void AuthController::registerUser(const httplib::Request &req, httplib::Response &res) {
    res.set_header("Content-Type", "application/json");
    
    json requestJson;
    try {
        requestJson = json::parse(req.body);
    } catch (const json::parse_error& e) {
        res.status = 400;
        json error = {{"error", "Invalid JSON format"}};
        res.set_content(error.dump(), "application/json");
        return;
    }
    
    json serviceResponse = AuthService::registerUser(requestJson);
    
    if (serviceResponse.contains("error")) {
        res.status = 400;
        res.set_content(serviceResponse.dump(), "application/json");
        return;
    }
    
    res.status = 201;
    res.set_content(serviceResponse.dump(), "application/json");
}
