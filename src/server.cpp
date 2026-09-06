#include <cstring>
#include <netdb.h>
#include <print>
#include <source_location>
#include <sys/socket.h>

#include "http.hpp"
#include "util.hpp"
#include "server.hpp"

constexpr uint16_t PORT = 8096;

namespace srv {

// private
template <typename F>
bool try_srv_func(
    F&& func,
    std::source_location location = std::source_location::current()
);
} // namespace srv

srv::ServerState::~ServerState() {
    if (addr_info != nullptr) 
        freeaddrinfo(addr_info); // free the linked list
    if (client_addr != nullptr)
        delete client_addr;
    if (error != nullptr)
        delete error;
}

bool srv::init_srv() {
    // throw server_error("test");
    sockaddr_storage client_addr;
    addrinfo hints = addrinfo{
        .ai_flags = AI_PASSIVE,     // fill the ip for me
        .ai_family = AF_INET,       // IPv4
        .ai_socktype = SOCK_STREAM, // TCP stream sockets
    };

    addrinfo *srv_info =
        http::get_addr_info(nullptr, std::to_string(PORT).c_str(), hints);

    util::devprint("SERVER:");
    for (addrinfo *p = srv_info; p != nullptr; p = p->ai_next)
        http::print_addr_info(*p);

    // use try_srv_func
    int srv_fd = ::socket(srv_info->ai_family, srv_info->ai_socktype,
                          srv_info->ai_protocol);
    if (srv_fd == -1) {
        int error = errno;
        std::println(stderr, "socket failed: {} (errno={})",
                     std::strerror(error),
                     error);
        return false;
    }
    util::devprint("File descriptor created. srv_fd={}", srv_fd);

    int yes = 1;
    setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)); // fix "Address already in use" error

    if (!srv::bind(srv_fd, srv_info))
        return false;

    if (!srv::listen(srv_fd, SOMAXCONN))
        return false;

    std::println("Listening on port {}", PORT);

    // cleanup
    freeaddrinfo(srv_info); // free the linked list

    // uncomment and return after you figure out how to deal with state pointers
    // ServerState state = ServerState(serv_info);
    return true;
}

bool srv::bind(int srv_fd, const addrinfo *srv_addr) {
    int res = ::bind(srv_fd, srv_addr->ai_addr, srv_addr->ai_addrlen);
    if (res == -1) {
        int error = errno; 
        std::println(stderr, "bind failed: {} (errno{})",
                     std::strerror(error),
                     error);
        return false;
    }
    util::devprint("Bind success. srv_fd={}", srv_fd);
    return true; 
}

bool srv::listen(int srv_fd, int n_conn) {
    int res = ::listen(srv_fd, n_conn);
    if (res == -1) { 
        int error = errno;
        std::println(stderr, "listen failed: {} (errno{})",
                     std::strerror(error), error);
        return false;
    }
    util::devprint("Listen success. srv_fd={}", srv_fd);
    return true;
}

bool srv::connect(int client_fd, const addrinfo *srv_addr) {
    int res = ::connect(client_fd, srv_addr->ai_addr, srv_addr->ai_addrlen);
    if (res == -1) {
        int error = errno;
        std::println(stderr, "connection failed: {} (errno{})",
                     std::strerror(error),
                     error);
        return false;
    }
    util::devprint("Connection success. sock_fd={}", client_fd);
    return true;
}

bool srv::accept(int srv_fd, sockaddr_storage &client_addr) {
    socklen_t client_len = sizeof(client_addr);
    int res = ::accept(srv_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
    if (res == -1) {
        int error = errno;
        std::println(stderr, "connection failed: {} (errno{})",
                     std::strerror(error),
                     error);
        return false;
    }
    util::devprint("Connection accepted. sock_fd={}", srv_fd);
    return true;
}

char* srv::parse(char line[], const char symbol[]) {
    char *copy = (char *)malloc(strlen(line) + 1);
    if (copy == NULL) 
        return NULL;

    strcpy(copy, line);

    strtok(copy, symbol);
    char *token = strtok(NULL, " ");
    const char *source;

    if (token == NULL)
        source = "";
    else 
        source = token;

    char *message= (char *)malloc(strlen(source) + 1);
    if (message != NULL) 
        strcpy(message, source);

    free(copy);

    return message;
}

// private
namespace srv {
template <typename F>
bool try_srv_func(
    F&& func,
    std::source_location location
) {
    int result = func();
    if (result == -1) {
        int error = errno;
        std::println(stderr, "{}:{} failed, {} (errno{}) [{}]",
                     location.file_name(), location.line(),
                     std::strerror(error), error, location.function_name());
        return false;
    }
    return result != 1;
}
} // namespace srv
