#include <cstring>
#include <netdb.h>
#include <print>
#include <sys/socket.h>

#include "server.hpp"

constexpr uint16_t PORT = 8001;

namespace srv {

bool init(const addrinfo& addr_info) {
    // throw server_error("test");
    int srv_fd = socket(addr_info.ai_family, addr_info.ai_socktype, 0);
    std::println("File descriptor created");

    std::println("Running HTTP server on port {}", PORT);
    return true;
}

char *parse(char line[], const char symbol[]) {
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

} // namespace srv
