#pragma once

#include <cstddef>
#include <arpa/inet.h>
// #include <sys/socket.h>

namespace http {

// sockaddr_storage storage{};

struct AddrInfo {
    int              flags = 0;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              family = AF_UNSPEC;    // AF_INET, AF_INET6, AF_UNSPEC
    int              socktype = SOCK_STREAM;  // SOCK_STREAM, SOCK_DGRAM
    int              protocol = 0;  // use 0 for "any"
    size_t           addrlen = 0;   // size of ai_addr in bytes
    struct SockAddr *addr = nullptr;      // struct sockaddr_in or _in6
    char            *cannonname = nullptr; // full canonical hostname

    struct AddrInfo *next = nullptr;      // linked list, next node

    void display_addr_info();
};

struct InAddr {
   uint32_t addr; 
};

struct SockAddr {
    unsigned short  family;
    char            data[14];
};

struct SockAddrIn {
    short int          family;  // Address family, AF_INET
    unsigned short int port;    // Port number
    struct InAddr      addr;    // Internet address
    unsigned char      zero[8]; // Same size as struct sockaddr
};

struct In6Addr {
    unsigned char   addr[16];   // IPv6 address
};

struct SockAddrIn6 {
    u_int16_t       family;   // address family, AF_INET6
    u_int16_t       port;     // port, Network Byte Order
    u_int32_t       flowinfo; // IPv6 flow information
    struct In6Addr  addr;     // IPv6 address
    u_int32_t       scope_id; // Scope ID
};

struct SockAddrStorage {
    sa_family_t  family;     // address family

    // all this is padding, implementation specific, ignore it:
    // char      __pad1[_SS_PAD1SIZE];
    // int64_t   __align;
    // char      __pad2[_SS_PAD2SIZE];
};

}

