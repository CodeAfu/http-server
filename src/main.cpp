// #include "util.hpp"
#include <cstring>
#include <print>
#include <netdb.h>
#include <arpa/inet.h>

#include "server.hpp"
#include "http.hpp"
#include "util.hpp"

void sample();

int main(int argc, char *argv[]) {
    std::println("Initalizing...");

    addrinfo addr_info = addrinfo{
        .ai_flags = AI_PASSIVE, // fill the ip for me
        .ai_family = AF_INET, // IPv4
        .ai_socktype = SOCK_STREAM, // TCP stream sockets
    };

    in6_addr ia6 = IN6ADDR_ANY_INIT;

    sockaddr_in sa{};
    char ipv4_text[INET_ADDRSTRLEN];
    // util::get_ipv4(sa, "192.0.2.3", ipv4_text, sizeof(ipv4_text));
    sa.sin_addr.s_addr = INADDR_ANY;
    inet_ntop(AF_INET, &(sa.sin_addr), ipv4_text, INET_ADDRSTRLEN);

    sockaddr_in6 sa6{};
    char ipv6_text[INET6_ADDRSTRLEN];
    // util::get_ipv6(sa6, "2001:db8:5413:4028::9db9", ipv6_text, sizeof(ipv6_text));
    sa6.sin6_addr = IN6ADDR_ANY_INIT;
    inet_ntop(AF_INET6, &(sa6.sin6_addr), ipv6_text, INET6_ADDRSTRLEN);


    std::println("ipv4={}", ipv4_text);
    std::println("ipv6={}", ipv6_text);

    sample();

    return util::tryCatch([&addr_info] {
        if (!srv::init(addr_info)) {
            return 1;
        }
        addrinfo* addr_results = http::display_addr_info(addr_info);
        freeaddrinfo(addr_results);
        return 0;
    });
}

void sample() {
    int status;
    addrinfo hints;
    addrinfo* servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo("www.example.net", "3490", &hints, &servinfo);
    
    for (addrinfo* curr = servinfo; curr == nullptr; curr = curr->ai_next) {
         
    }

    freeaddrinfo(servinfo);
}
