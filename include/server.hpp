#pragma once

#include <stdexcept>

struct addrinfo;

namespace srv {

class server_error : public std::runtime_error {
public:
    explicit server_error(const std::string& msg)
        : std::runtime_error(msg) {}
};

struct ServerState {
    addrinfo* m_addrinfo;
    bool m_success;
    std::string* error;
    int errno;

    ~ServerState();
};

bool init_srv();
bool conn(int sock_fd, addrinfo *srv);
char* parse(char line[], const char symbol[]);

} // namespace srv
