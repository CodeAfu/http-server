#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <print>

#include "http.hpp"

addrinfo *http::get_addr_info(const char *host, const char* port, const ::addrinfo &hints) {
    addrinfo *results = nullptr;
    int error = getaddrinfo(host, port, &hints, &results);
    if (error != 0) {
        std::println(stderr, "getaddrinfo failed: {}", gai_strerror(error));
        return nullptr;
    }
    return results;
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

    std::println(
        "{} {}:{} socktype={} protocol={} flags={} canon={} addrlen={}", family,
        host, service, addr_info.ai_socktype, addr_info.ai_protocol,
        addr_info.ai_flags,
        addr_info.ai_canonname ? addr_info.ai_canonname : "(none)",
        addr_info.ai_addrlen);
}
