// #include "util.hpp"
#include <print>
#include <netdb.h>

#include "server.hpp"
#include "http.hpp"
#include "util.hpp"

int main() {
    std::println("Initalizing...");

    addrinfo addr_info = addrinfo{
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };

    // in6_addr ia6 = IN6ADDR_ANY_INIT;

    sockaddr_in sa{};
    char ipv4_text[INET_ADDRSTRLEN];
    util::get_ipv4(sa, "127.0.0.1", ipv4_text, sizeof(ipv4_text));

    sockaddr_in6 sa6{};
    char ipv6_text[INET6_ADDRSTRLEN];
    util::get_ipv6(sa6, "::1", ipv6_text, sizeof(ipv6_text));

    std::println("ipv4={}", ipv4_text);
    std::println("ipv6={}", ipv6_text);

    return util::tryCatch([&addr_info] {
        srv::init(addr_info);
        http::display_addr_info(addr_info);
    });
}
