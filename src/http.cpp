#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <print>

#include "http.hpp"
#include "util.hpp"

addrinfo* http::get_addr_info(const char *host, const char* port, const ::addrinfo &hints) {
    addrinfo *results = nullptr;
    int error = getaddrinfo(host, port, &hints, &results);
    if (error != 0) {
        std::println(stderr, "getaddrinfo failed: {}", gai_strerror(error));
        return nullptr;
    }
    return results;
}

void* http::get_in_addr(::sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in *)sa)->sin_addr);
    } else if (sa->sa_family == AF_INET6) {
        return &(((sockaddr_in6 *)sa)->sin6_addr);
    }
    std::println(stderr, "get_in_addr error: no sockinaddr found.");
    return nullptr;
}


void http::print_addr_info(const ::addrinfo &addr_info) {
    char host[NI_MAXHOST]{};
    char service[NI_MAXSERV]{};

    int error =
        getnameinfo(addr_info.ai_addr, addr_info.ai_addrlen, host, sizeof(host),
                    service, sizeof(service), NI_NUMERICHOST | NI_NUMERICSERV);

    if (error != 0) {
        std::println(stderr, "getnameinfo failed: {}", gai_strerror(error));
        return;
    }

    const char *family = "Unknown";
    if (addr_info.ai_family == AF_INET) {
        family = "IPv4";
    } else if (addr_info.ai_family == AF_INET6) {
        family = "IPv6";
    }

    util::devprint(
        "{} {}:{} socktype={} protocol={} flags={} canon={} addrlen={}", family,
        host, service, addr_info.ai_socktype, addr_info.ai_protocol,
        addr_info.ai_flags,
        addr_info.ai_canonname ? addr_info.ai_canonname : "(none)",
        addr_info.ai_addrlen);
}

bool send_msg(int sock_fd, const std::string& msg, int flags = 0) {
    std::size_t total = 0;
    while (total < msg.size()) {
        ssize_t sent = ::send(
            sock_fd,
            msg.data() + total,
            msg.size() - total,
            flags
        );
        
        if (sent == -1) {
            if (errno == EINTR) {
                continue;
            }
            return false;
        }

        if (sent == 0) {
            return false;
        }

        total += static_cast<std::size_t>(sent);
    }
    return true;
}

const std::string& recv_msg(int sock_fd, std::string& msg, int flags = 0) {
    ssize_t received = ::recv(sock_fd, msg.data(), msg.size(), flags);
    if (received <= 0)
        msg.clear();

    msg.resize(received);
    return msg;
}
