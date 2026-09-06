#include <arpa/inet.h>

#include "util.hpp"

void util::get_ipv4(sockaddr_in &sa, const char* addr, char *out, size_t out_len) {
    inet_pton(AF_INET, addr, &(sa.sin_addr));
    inet_ntop(AF_INET, &sa.sin_addr, out, out_len);
}

void util::get_ipv6(sockaddr_in6 &sa6, const char* addr, char *out, size_t out_len) {
    inet_pton(AF_INET6, addr, &(sa6.sin6_addr));
    inet_ntop(AF_INET6, &sa6.sin6_addr, out, out_len);
}
