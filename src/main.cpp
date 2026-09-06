// #include "util.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <print>

#include "http.hpp"
#include "server.hpp"
#include "util.hpp"

void sample();

int main(int argc, char *argv[]) {
    sample();

    in6_addr ia6 = IN6ADDR_ANY_INIT;

    sockaddr_in sa{};
    char ipv4_text[INET_ADDRSTRLEN];
    // util::get_ipv4(sa, "192.0.2.3", ipv4_text, sizeof(ipv4_text));
    sa.sin_addr.s_addr = INADDR_ANY;
    inet_ntop(AF_INET, &(sa.sin_addr), ipv4_text, INET_ADDRSTRLEN);

    sockaddr_in6 sa6{};
    char ipv6_text[INET6_ADDRSTRLEN];
    // util::get_ipv6(sa6, "2001:db8:5413:4028::9db9", ipv6_text,
    // sizeof(ipv6_text));
    sa6.sin6_addr = IN6ADDR_ANY_INIT;
    inet_ntop(AF_INET6, &(sa6.sin6_addr), ipv6_text, INET6_ADDRSTRLEN);

    return util::tryCatch([] {
        if (!srv::init_srv()) {
            return 1;
        }
        return 0;
    });
}

void sample() {
    util::devprint("---  SAMPLE  ---");
    addrinfo hints;
    addrinfo *servinfo = nullptr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo("projectzoku.site", "443", &hints, &servinfo);
    if (status != 0) {
        std::println(stderr, "fail: {}", gai_strerror(status));
        return;
    }

    for (addrinfo *p = servinfo; p != nullptr; p = p->ai_next) {
        http::print_addr_info(*p);
    }

    freeaddrinfo(servinfo);
    util::devprint("---  /SAMPLE  ---");
}
