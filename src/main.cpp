#include <drogon/drogon.h>
#include <iostream>

using namespace drogon;

int main() {
    std::cout << "=====================================" << std::endl;
    std::cout << "           KANYAMART SERVER           " << std::endl;
    std::cout << "=====================================" << std::endl;

    // Load configuration
    app().loadConfigFile("./config.json");

    // CORS filter for API requests
    app().registerPreHandlingAdvice(
        [](const HttpRequestPtr &req, AdviceCallback &&acb, AdviceChainCallback &&accb) {
            // Handle CORS preflight
            if (req->method() == Options) {
                auto resp = HttpResponse::newHttpResponse();
                resp->addHeader("Access-Control-Allow-Origin", req->getHeader("Origin").empty() ? "*" : req->getHeader("Origin"));
                resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
                resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
                resp->addHeader("Access-Control-Allow-Credentials", "true");
                resp->addHeader("Access-Control-Max-Age", "86400");
                resp->setStatusCode(k204NoContent);
                acb(resp);
                return;
            }
            accb();
        }
    );

    // Add CORS headers to all responses
    app().registerPostHandlingAdvice(
        [](const HttpRequestPtr &req, const HttpResponsePtr &resp) {
            resp->addHeader("Access-Control-Allow-Origin", req->getHeader("Origin").empty() ? "*" : req->getHeader("Origin"));
            resp->addHeader("Access-Control-Allow-Credentials", "true");
        }
    );

    // Fallback for SPA routing - serve index.html for unknown routes
    app().setCustom404Page(
        HttpResponse::newFileResponse("./web/index.html", "", drogon::CT_TEXT_HTML)
    );

    std::cout << "[INFO] Starting KanyaMart on port 8080..." << std::endl;
    std::cout << "[INFO] Frontend: http://localhost:8080" << std::endl;
    std::cout << "[INFO] API Base: http://localhost:8080/api/v1" << std::endl;

    app().run();
    return 0;
}
