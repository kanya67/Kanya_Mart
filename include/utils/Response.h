#pragma once
#include <json/json.h>
#include <string>

namespace Response {

inline Json::Value success(const Json::Value &data) {
    Json::Value res;
    res["success"] = true;
    res["data"] = data;
    return res;
}

inline Json::Value successList(const Json::Value &data, int page, int perPage, int total) {
    Json::Value res;
    res["success"] = true;
    res["data"] = data;
    res["pagination"]["page"] = page;
    res["pagination"]["per_page"] = perPage;
    res["pagination"]["total"] = total;
    res["pagination"]["total_pages"] = (total + perPage - 1) / perPage;
    return res;
}

inline Json::Value error(const std::string &code, const std::string &message) {
    Json::Value res;
    res["success"] = false;
    res["error"]["code"] = code;
    res["error"]["message"] = message;
    return res;
}

inline std::string toJsonString(const Json::Value &val) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, val);
}

} // namespace Response
