#pragma once

#include "http.hpp"
#include <stdexcept>

namespace srv {

class server_error : public std::runtime_error {
public:
    explicit server_error(const std::string& msg)
        : std::runtime_error(msg) {}
};

bool init(const http::AddrInfo& addr_info);
char* parse(char line[], const char symbol[]);

}
