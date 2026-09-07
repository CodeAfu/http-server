#include <cstring>
#include <netdb.h>
#include <print>
#include <source_location>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "http.hpp"
#include "util.hpp"
#include "server.hpp"

constexpr uint16_t PORT = 8096;

// private
namespace srv {
template <typename F>
bool try_srv_func(
    F&& func,
    std::source_location location = std::source_location::current()
);
} // namespace srv

void srv::run(Server& s) {
    std::println("Running server");
    sockaddr_storage client_fd;

    while (true) {
        ::sleep(1);
        util::devprint("poll");
    }

    std::println("Closing in 3 seconds...");
    ::sleep(3); // POSIX
}

srv::Server srv::init() {
    // throw server_error("test");
    Server server{};
    addrinfo hints = addrinfo{
        .ai_flags = AI_PASSIVE,     // fill the ip for me
        .ai_family = AF_INET,       // IPv4
        .ai_socktype = SOCK_STREAM, // TCP stream sockets
    };

    server.addr_info =
        http::get_addr_info(nullptr, std::to_string(PORT).c_str(), hints);

    util::devprint("SERVER:");
    for (addrinfo *p = server.addr_info; p != nullptr; p = p->ai_next)
        http::print_addr_info(*p);

    // use try_srv_func
    server.sock_fd = ::socket(server.addr_info->ai_family, server.addr_info->ai_socktype,
                          server.addr_info->ai_protocol);
    if (server.sock_fd == -1) {
        server.error_no = errno;
        server.error = std::strerror(server.error_no);
        std::println(stderr, "socket failed: {} (errno={})",
                     server.error,
                     server.error_no);
        return server;
    }
    util::devprint("File descriptor created. srv_fd={}", server.sock_fd);

    int yes = 1;
    int sock_opt = ::setsockopt(server.sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)); // fix "Address already in use" error
    if (sock_opt == -1) {
        server.error_no = errno;
        server.error = std::strerror(server.error_no);
        std::println(stderr, "setsockopt failed: {} (errno={})",
                     server.error,
                     server.error_no);
        return server;
    }

    if (!srv::bind(server))
        return server;

    if (!srv::listen(server))
        return server;

    std::println("Listening on port {}", PORT);

    server.success = true;
    return server;
}


bool srv::bind(Server& s) {
    if (s.addr_info == nullptr) {
        s.error = "No address info avaiable";
        return false;
    }

    int res = ::bind(s.sock_fd, s.addr_info->ai_addr, s.addr_info->ai_addrlen);

    if (res == -1) {
        s.error_no = errno; 
        s.error = std::strerror(s.error_no);
        std::println(stderr, "bind failed: {} (errno={})",
                     s.error,
                     s.error_no);
        return false;
    }
    util::devprint("Bind success. srv_fd={}", s.sock_fd);
    return true; 
}

bool srv::listen(Server& s) {
    int res = ::listen(s.sock_fd, s.n_conn);
    if (res == -1) { 
        int error = errno;
        std::println(stderr, "listen failed: {} (errno={})",
                     std::strerror(error), error);
        return false;
    }
    util::devprint("Listen success. srv_fd={}", s.sock_fd);
    return true;
}

bool srv::connect(int client_fd, const ::addrinfo *srv_addr) {
    int res = ::connect(client_fd, srv_addr->ai_addr, srv_addr->ai_addrlen);
    if (res == -1) {
        int error = errno;
        std::println(stderr, "connection failed: {} (errno{})",
                     std::strerror(error),
                     error);
        return false;
    }
    util::devprint("Connection success. client_fd={}", client_fd);
    return true;
}

bool srv::accept(Server& s, ::sockaddr_storage& client_addr) {
    socklen_t client_len = sizeof(client_addr);
    int client_fd = ::accept(s.sock_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
    if (client_fd == -1) {
        int error = errno;
        s.error = std::strerror(error);
        std::println(stderr, "connection failed: {} (errno={})",
                     std::strerror(error),
                     error);
        return false;
    }
    util::devprint("Connection accepted. client_fd={}", client_fd);
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

srv::Server::~Server() {
    util::devprint("Server destructor called");
    if (sock_fd != -1)
        ::close(sock_fd);
    if (addr_info != nullptr) 
        freeaddrinfo(addr_info); // free the linked list
}

// private
namespace srv {
void sigchld_handler(int s) {
    (void)s;
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

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

