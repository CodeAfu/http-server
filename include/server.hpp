#pragma once

#include <stdexcept>

struct addrinfo;

namespace srv {

class server_error : public std::runtime_error {
public:
    explicit server_error(const std::string& msg)
        : std::runtime_error(msg) {}
};

bool init(const addrinfo& addr_info);
char* parse(char line[], const char symbol[]);

}
