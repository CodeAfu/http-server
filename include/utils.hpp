#pragma once

#include "server.hpp"
#include <concepts>
#include <print>
#include <utility>

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

}; // namespace util
