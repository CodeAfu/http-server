#pragma once

#include <cstdlib>
#include <cstring>
#include <concepts>
#include <print>
#include <utility>
#include <cstddef>

#include "server.hpp"

struct sockaddr_in;
struct sockaddr_in6;

namespace util {
template <std::invocable F>
int tryCatch(F&& func) {
    try {
        std::forward<F>(func)();
    } catch (const srv::server_error& e) {
        std::println(stderr, "Server error caught: {}", e.what());
        return 1;
    } catch (const std::exception& e) {
        std::println(stderr, "Standard exception: {}", e.what());
        return 1;
    } catch (...) {
        std::println(stderr, "Unknown non-standard exception caught");
        return 1;
    }
    return 0;
}

template <typename... Args>
inline void devprint(std::format_string<Args...> fmt, Args&&... values) {
    const char* env = std::getenv("ENV");
    if (env == nullptr || std::strcmp(env, "development") != 0) {
        return;
    }
    std::print("\x1b[33m");
    std::print(fmt, std::forward<Args>(values)...);
    std::println("\x1b[0m");
}

void get_ipv4(sockaddr_in &sa, const char* addr, char *out, size_t out_len);
void get_ipv6(sockaddr_in6 &sa6, const char* addr, char *out, size_t out_len);
const char* inet_ntop(void *sa, const char* addr, char *out, size_t out_len);
}; // namespace util
