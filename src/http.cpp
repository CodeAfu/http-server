#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <print>
#include <ranges>

#include "base.h"
#include "http.hpp"
#include "util.hpp"

#define pack754_32(f) (pack754(f), 32, 8)
#define pack754_64(f) (pack754(f), 64, 11)
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

AddressInfo* http::get_addr_info(const char *host, const char* port, const ::AddressInfo &hints)
{
    AddressInfo *results = nullptr;
    int error = getaddrinfo(host, port, &hints, &results);
    if (error != 0) {
        std::println(stderr, "getaddrinfo failed: {}", gai_strerror(error));
        return nullptr;
    }
    return results;
}

void* http::get_in_addr(::SocketAddress *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((SocketAddressIPv4 *)sa)->sin_addr);
    } else if (sa->sa_family == AF_INET6) {
        return &(((SocketAddressIPv6 *)sa)->sin6_addr);
    }
    std::println(stderr, "get_in_addr error: no sockinaddr found.");
    return nullptr;
}


void http::print_addr_info(const ::AddressInfo &addr_info)
{
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

bool http::send_msg(int sock_fd, const std::string& msg, int flags = 0)
{
    std::size_t total = 0;
    while (total < msg.size()) {
        ssize_t sent = ::send(sock_fd, 
                              msg.data() + total,
                              msg.size() - total,
                              flags);
        
        if (sent > 0) {
            total += static_cast<std::size_t>(sent);
            continue;
        }

        if (sent == -1 && errno == EINTR) {
            continue;
        }

        return false;
    }

    return true;
}

const std::string& http::recv_msg(int sock_fd, std::string& msg, int flags = 0)
{
    ssize_t received = ::recv(sock_fd, msg.data(), msg.size(), flags);
    if (received <= 0)
        msg.clear();

    msg.resize(received);
    return msg;
}

uint32_t http::htonf(float f)
{
    uint32_t p;
    uint32_t sign;

    if (f < 0) {
        sign = 1;
        f = -f;
    } else {
        sign = 0;
    }

    /// some dirty stuff wao
    // whole part and sign
    p = ((((uint32_t)f)&0x7fff)<<16) | (sign<<31);

    // fraction
    p |= (uint32_t)(((f - (int)f) * 65536.0f))&0xffff; // 65536 = 2^16

    return p;
}

float http::ntohf(uint32_t p)
{
    float f = ((p >> 16)&0x7fff);
    f += (p&0xffff) / 65536.0f;

    if (((p >> 31)&0x1) == 0x1)
        f = -f;

    return f;
}

uint64_t pack754(long double f, unsigned bits, unsigned expbits)
{
    long double fnorm;
    int shift;
    long long sign, exp, significand;

    // -1 for sign bit
    unsigned significandbits = bits - expbits - 1;

    if (f == 0.0) return 0; // get this special case out of the way

    // check sign and begin normalization
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }

    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL<<significandbits) + 0.5f);

    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

long double unpack754(uint64_t i, unsigned bits, unsigned expbits)
{
    long double result;
    long long shift;
    unsigned bias;

    // -1 for sign bit
    unsigned significandbits = bits - expbits - 1;

    if (i == 0) return 0.0;

    // pull the significand
    result = (i&((1LL<<significandbits)-1)); // mask
    result /= (1LL<<significandbits); // convert back to float
    result += 1.0f; // add the one back on

    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }

    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;

    return result;
}

HttpRequest http::get_http_request(std::string_view raw_request)
{
    auto lines = raw_request | std::views::split(std::string("\r\n"));
    for (auto part : lines) {
        std::string line(part.begin(), part.end());
    }
}

std::string http::create_http_response(HttpResponse resp)
{
    std::string payload = std::format(
        "HTTP/1.1 {} {}\r\n"
        "Content-Type: {}\r\n"
        "Content-Length: {}\r\n"
        "Connection: close\r\n",
        resp.status, resp.reason,
        ContentType::TEXT,
        resp.body.size()
    );

    for (const auto &[name, value] : resp.headers) {
        payload += std::format("{}: {}\r\n", name, value);
    }
    
    payload += "\r\n";
    payload += resp.body;

    return payload;
}
