#include "http.hpp"
#include <arpa/inet.h>
#include <print>

namespace http {

void http::AddrInfo::display_addr_info() {
    char ipstr[INET6_ADDRSTRLEN] = {0};
    const void *network_address = nullptr;
    const char *family_str = nullptr;
    int port = 0;

    if (addr == nullptr) {
        std::println("No address available");
        return;
    }

    if (family == AF_INET) {
        auto *ipv4 = reinterpret_cast<const sockaddr_in *>(addr);
        network_address = &ipv4->sin_addr;
        port = ntohs(ipv4->sin_port);
        family_str = "IPv4";
    } else if (family == AF_INET6) {
        auto *ipv6 = reinterpret_cast<const SockAddrIn6 *>(addr);
        network_address = &ipv6->sin6_addr;
        port = ntohs(ipv6->sin6_port);
        family_str = "IPv6";
    } else {
        std::println("Unknown family: {}", family);
        return;
    }

    if (inet_ntop(family, network_address, ipstr, sizeof(ipstr)) == nullptr) {
        std::println("Could not convert address to text");
        return;
    }

    std::println(
        "{} {}:{}  socktype={} protocol={} flags={} canon={} addrlen={}",
        family_str, ipstr, port, socktype, protocol, flags,
        cannonname ? cannonname : "(none)", addrlen);
}

} // namespace http
