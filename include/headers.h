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

enum StatusCode {
    OK = 200,
    CREATED = 201,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
};

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

namespace header {
static inline std::string status_reason(StatusCode code)
{
    switch (code) {
        case OK:
            return "OK";
        case CREATED:
            return "Created";
        case BAD_REQUEST:
            return "Bad Request";
        case UNAUTHORIZED:
            return "Unauthorized";
        case NOT_FOUND:
            return "Not Found";
        case INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        case NOT_IMPLEMENTED:
            return "Not Implemented";
        case BAD_GATEWAY:
            return "Bad Gateway";
        default:
            return "";
    }
}
} // namespace header
