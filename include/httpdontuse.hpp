// JUST FOR REFERENCE. DONT USE THIS!!!
#pragma once

#include <cstddef>
#include <netinet/in.h>
// #include <sys/socket.h>

namespace httpdontuse {

// sockaddr_storage storage{};

struct AddrInfo {
    int ai_flags = 0;                   // AI_PASSIVE, AI_CANONNAME, etc.
    int ai_family = AF_UNSPEC;          // AF_INET, AF_INET6, AF_UNSPEC
    int ai_socktype = SOCK_STREAM;      // SOCK_STREAM, SOCK_DGRAM
    int ai_protocol = 0;                // use 0 for "any"
    size_t ai_addrlen = 0;              // size of ai_addr in bytes
    struct SockAddr *ai_addr = nullptr; // struct sockaddr_in or _in6
    char *ai_cannonname = nullptr;      // full canonical hostname

    struct AddrInfo *ai_next = nullptr; // linked list, next node

    void display_addr_info();
};

struct SockAddr {
    unsigned short sa_family;
    char sa_data[14];
};

struct InAddr {
    uint32_t ia_addr;
};

struct SockAddrIn {
    short int sin_family;        // Address family, AF_INET
    unsigned short int sin_port; // Port number
    struct InAddr sin_addr;      // Internet address
    unsigned char sin_zero[8];   // Same size as struct sockaddr
};

struct In6Addr {
    unsigned char ia6_addr[16]; // IPv6 address
};

struct SockAddrIn6 {
    u_int16_t sin6_family;    // address family, AF_INET6
    u_int16_t sin6_port;      // port, Network Byte Order
    u_int32_t sin6_flowinfo;  // IPv6 flow information
    struct In6Addr sin6_addr; // IPv6 address
    u_int32_t sin6_scope_id;  // Scope ID
};

struct SockAddrStorage {
    sa_family_t ss_family; // address family

    // all this is padding, implementation specific, ignore it:
    // char      __pad1[_SS_PAD1SIZE];
    // int64_t   __align;
    // char      __pad2[_SS_PAD2SIZE];
};

} // namespace http
