#pragma once

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

void get_ipv4(sockaddr_in &sa, const char* addr, char *out, size_t out_len);
void get_ipv6(sockaddr_in6 &sa6, const char* addr, char *out, size_t out_len);

}; // namespace util
