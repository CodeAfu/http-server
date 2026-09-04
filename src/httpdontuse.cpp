#include "httpdontuse.hpp"
#include <arpa/inet.h>
#include <print>

void httpdontuse::AddrInfo::display_addr_info() {
    char ipstr[INET6_ADDRSTRLEN] = {0};
    const void *network_address = nullptr;
    const char *family_str = nullptr;
    int port = 0;

    if (ai_addr == nullptr) {
        std::println("No address available");
        return;
    }

    if (ai_family == AF_INET) {
        auto *ipv4 = reinterpret_cast<const SockAddrIn *>(ai_addr);
        network_address = &ipv4->sin_addr.ia_addr;
        port = ntohs(ipv4->sin_port);
        family_str = "IPv4";
    } else if (ai_family == AF_INET6) {
        auto *ipv6 = reinterpret_cast<const SockAddrIn6 *>(ai_addr);
        network_address = &ipv6->sin6_addr.ia6_addr;
        port = ntohs(ipv6->sin6_port);
        family_str = "IPv6";
    } else {
        std::println("Unknown family: {}", ai_family);
        return;
    }

    if (inet_ntop(ai_family,
                  network_address, 
                  ipstr, 
                  sizeof(ipstr)) == nullptr) {
        std::println("Could not convert address to text");
        return;
    }

    std::println(
        "{} {}:{}  socktype={} protocol={} flags={} canon={} addrlen={}",
        family_str, ipstr, port, ai_socktype, ai_protocol, ai_flags,
        ai_cannonname ? ai_cannonname : "(none)", ai_addrlen);
}
