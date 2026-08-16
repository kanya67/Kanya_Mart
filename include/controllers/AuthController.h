#pragma once
#include <httplib.h>

class AuthController {
public:
    static void initRoutes(httplib::Server &svr);
    static void registerUser(const httplib::Request &req, httplib::Response &res);
};
