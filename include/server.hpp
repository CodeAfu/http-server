#pragma once

#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>

#include "base.h"

namespace srv {
class server_error : public std::runtime_error {
public:
    explicit server_error(const std::string &msg) : std::runtime_error(msg) {}
};

struct Server {
    static constexpr std::size_t PORT = 8096;
    static constexpr std::size_t MAX_CLIENTS = 10; // <sys/socket.h> SOMAXCONN = 4096
    static constexpr std::size_t POLLFD_TIMEOUT = 1000;
    static constexpr int BACKLOG = 10;

    AddressInfo *address_info = nullptr;
    int socket_fd = -1;
    PollFD pfds[MAX_CLIENTS + 2]{};
    std::size_t pfds_size = MAX_CLIENTS + 1;
    // std::size_t pfds_count = 0;

    bool success = false;
    std::string error;
    int error_no = 0;

    ~Server();
};

// struct ClientConnection {
//     int fd;
//     sockaddr_storage address;
// };

// void add_to_pfds(Server &s, int new_pfd);
Server init();
void run(Server &s);
bool create_socket(Server &s);
bool bind(Server &s);
bool listen(Server &s);
bool connect(int client_fd, const AddressInfo *srv_addr);
int accept(Server &s, SocketAddressStorage &client_addr);
void *get_in_addr(SocketAddress *sa);
char *parse(char line[], const char symbol[]);
} // namespace srv
