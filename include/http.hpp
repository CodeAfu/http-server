#pragma once

#include <cstdint>
#include <string>

#include "headers.h"

struct addrinfo;
struct sockaddr;

namespace http {
addrinfo* get_addr_info(const char *host, const char *port, const ::addrinfo& hints);
void* get_in_addr(::sockaddr *sa);
void print_addr_info(const ::addrinfo& addr_info);

bool send_msg(int sock_fd, const std::string& msg, int flags);
const std::string& recv_msg(int sock_fd, std::string& msg, int flags);
uint32_t htonf(float f);
float ntohf(uint32_t p);
uint64_t pack754(long double f, unsigned bits, unsigned expbits);
long double unpack754(uint64_t i, unsigned bits, unsigned expbits);

HttpRequest get_http_request();
HttpResponse create_http_response();
} // namespace http
