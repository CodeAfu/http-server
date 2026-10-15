#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>

namespace ContentType {
inline constexpr std::string_view TEXT = "text/plain";
inline constexpr std::string_view JSON = "application/json";
} // namespace ContentType

namespace MethodType {
inline constexpr std::string_view GET = "GET";
inline constexpr std::string_view POST = "POST";
inline constexpr std::string_view DELETE = "DELETE";
inline constexpr std::string_view PUT = "PUT";
} // namespace MethodType

struct HttpHeader {
    std::string name;
    std::string value;
};

struct HttpRequest {
    std::string method; // "GET"
    std::string target; // "/index.html"
    std::string version; // "HTTP/1.1"

    std::vector<HttpHeader> headers;
    std::string body;
};

struct HttpResponse {
    uint16_t status; // 200, 404
    const std::string reason; // "OK", "Not Found"
    const std::string version; // "HTTP/1.1"

    std::vector<HttpHeader> headers;
    const std::string body;
};
