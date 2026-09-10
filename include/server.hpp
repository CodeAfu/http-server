#pragma once

#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>

struct addrinfo;
struct sockaddr_storage;

namespace srv {

class server_error : public std::runtime_error {
public:
    explicit server_error(const std::string& msg)
        : std::runtime_error(msg) {}
};

struct Server {
    int sock_fd = -1;
    ::addrinfo* addr_info = nullptr;
    int max_conn = 10; // <sys/socket.h> SOMAXCONN = 4096
    pollfd pfds[12];
    bool success = false;
    std::string error;
    int error_no = 0;

    ~Server();
};

// struct ClientConnection {
//     int fd;
//     sockaddr_storage address;
// };

Server init();
void run(Server& s);
bool bind(Server& s);
bool listen(Server& s);
bool connect(int client_fd, const ::addrinfo *srv_addr);
int accept(Server& s, ::sockaddr_storage& client_addr);
void* get_in_addr(::sockaddr *sa);
char* parse(char line[], const char symbol[]);

} // namespace srv
