#include <cstring>
#include <netdb.h>
#include <print>
#include <source_location>
#include <sys/socket.h>

#include "http.hpp"
#include "util.hpp"
#include "server.hpp"

constexpr uint16_t PORT = 8096;

template <typename F>
bool try_srv_func(
    F&& func,
    std::source_location location = std::source_location::current()
);

srv::ServerState::~ServerState() {
    if (m_addrinfo != nullptr) 
        freeaddrinfo(m_addrinfo); // free the linked list
}

bool srv::init_srv() {
    // throw server_error("test");
    addrinfo hints = addrinfo{
        .ai_flags = AI_PASSIVE,     // fill the ip for me
        .ai_family = AF_INET,       // IPv4
        .ai_socktype = SOCK_STREAM, // TCP stream sockets
    };
    std::string port_str = std::to_string(PORT);

    addrinfo *serv_info =
        http::get_addr_info(nullptr, port_str.c_str(), hints);

    for (addrinfo *p = serv_info; p != nullptr; p = p->ai_next) {
        http::print_addr_info(*p);
    }

    // use try_srv_func
    int sock_fd = socket(serv_info->ai_family, serv_info->ai_socktype,
                         serv_info->ai_protocol);
    if (sock_fd == -1) {
        int error = errno;
        std::println(stderr, "socket failed: {} (errno={})",
                     std::strerror(error),
                     error);
        return false;
    }
    util::devprint("File descriptor created");

    int yes = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes); // fix "Address already in use" error

    // use try_srv_func
    int bind_result = bind(sock_fd, serv_info->ai_addr, serv_info->ai_addrlen);
    if (bind_result == -1) {
        int error = errno;
        std::println(stderr, "bind failed: {} (errno{})",
                     std::strerror(error),
                     error);
        return false;
    }
    util::devprint("Bind success");

    int l = listen(sock_fd, SOMAXCONN);
    if (l == -1) { 
        int error = errno;
        std::println(stderr, "listen failed: {} (errno{})",
                     std::strerror(error), error);
    }
    std::println("Listening on port {}", PORT);

    // int conn_result = connect(sock_fd, serv_info->ai_addr, serv_info->ai_addrlen);
    // if (conn_result == -1) {
    //     int error = errno;
    //     std::println(stderr, "connection failed: {} (errno{})",
    //                  std::strerror(error),
    //                  error);
    //     return false;
    // }
    // util::devprint("Connection success");

    // if (!try_srv_func([&] {
    //     return connect(sock_fd, serv_info->ai_addr, serv_info->ai_addrlen);
    // }))
    //     return false;

    // cleanup
    freeaddrinfo(serv_info); // free the linked list

    // Uncomment after you figure out how to deal with the pointers
    // ServerState state = ServerState(serv_info);
    return true;
}

bool srv::conn(int sock_fd, addrinfo *srv) {
    int res = connect(sock_fd, srv->ai_addr, srv->ai_addrlen);
    if (res == -1) {
        int error = errno;
        std::println(stderr, "connection failed: {} (errno{})",
                     std::strerror(error),
                     error);
        return false;
    }
    util::devprint("Connection success");
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
