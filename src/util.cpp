#include "util.hpp"

#include <arpa/inet.h>

void util::get_ipv4(sockaddr_in &sa, const char* addr, char *out, size_t out_len) {
    ::inet_pton(AF_INET, addr, &(sa.sin_addr));
    ::inet_ntop(AF_INET, &sa.sin_addr, out, out_len);
}

void util::get_ipv6(sockaddr_in6 &sa6, const char* addr, char *out, size_t out_len) {
    ::inet_pton(AF_INET6, addr, &(sa6.sin6_addr));
    ::inet_ntop(AF_INET6, &sa6.sin6_addr, out, out_len);
}

const char* util::inet_ntop(void *sa, const char* addr, char *buf, size_t size) {
    sockaddr_storage *sas = (sockaddr_storage*)addr;
    sockaddr_in *sa4 = nullptr;
    sockaddr_in6 *sa6 = nullptr;
    void *src;

    switch (sas->ss_family) {
        case (AF_INET):
            sa4 = (sockaddr_in*)addr;
            src = &(sa4->sin_addr);
            break;
        case (AF_INET6):
            sa6 = (sockaddr_in6*)addr;
            src = &(sa6->sin6_addr);
            break;
        default:
            return nullptr;
    }
    
    return ::inet_ntop(sas->ss_family, src, buf, size);
}
