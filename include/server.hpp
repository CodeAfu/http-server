#pragma once

#include <stdexcept>

struct addrinfo;
struct sockaddr_storage;

namespace srv {

class server_error : public std::runtime_error {
public:
    explicit server_error(const std::string& msg)
        : std::runtime_error(msg) {}
};

struct ServerState {
    addrinfo* addr_info = nullptr;
    sockaddr_storage* client_addr = nullptr;
    bool success;
    std::string* error = nullptr;
    int error_no = 0;

    ~ServerState();
};

bool init_srv();
bool bind(int srv_fd, const addrinfo *addr_info);
bool listen(int srv_fd, int n_conn);
bool connect(int client_fd, const addrinfo *srv_addr);
bool accept(int srv_fd, sockaddr_storage &client_addr);
char* parse(char line[], const char symbol[]);

} // namespace srv
