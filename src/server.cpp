#include <cstring>
#include <netdb.h>
#include <print>
#include <sys/socket.h>

#include "http.hpp"
#include "server.hpp"

constexpr uint16_t PORT = 8001;

bool srv::init() {
    // throw server_error("test");
    addrinfo hints = addrinfo{
        .ai_flags = AI_PASSIVE,     // fill the ip for me
        .ai_family = AF_INET,       // IPv4
        .ai_socktype = SOCK_STREAM, // TCP stream sockets
    };
    std::string port_str = std::to_string(PORT);

    addrinfo *servinfo =
        http::get_addr_info("127.0.0.1", port_str.c_str(), hints);

    int srv_fd = socket(servinfo->ai_family, servinfo->ai_socktype,
                        servinfo->ai_protocol);
    if (srv_fd == -1) {
        int error = errno;
        std::println(stderr, "socket failed: {} (errno={})",
                     std::strerror(error), error);
    }
    std::println("File descriptor created");

    std::println("Running HTTP server on port {}", PORT);

    // cleanup
    freeaddrinfo(servinfo); // free the linked list
    return true;
}

char *srv::parse(char line[], const char symbol[]) {
    char *copy = (char *)malloc(strlen(line) + 1);
    strcpy(copy, line);

    char *message;
    char *token = strtok(copy, symbol);
    char current = 0;

    while (token != NULL) {
        token = strtok(NULL, " ");
        if (current == 0) {
            message = token;
            if (message == NULL) {
                message = "";
            }
            return message;
        }
        current = current + 1;
    }

    free(message);
    free(token);
    return message;
}
