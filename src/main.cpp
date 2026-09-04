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
    sockaddr_in sa{};
    sockaddr_in6 sa6{};
    char ipv4_text[INET_ADDRSTRLEN];
    char ipv6_text[INET6_ADDRSTRLEN];

    in6_addr ia6 = IN6ADDR_ANY_INIT;

    util::get_ipv4(sa, "10.12.110.57", ipv4_text, sizeof(ipv4_text));
    util::get_ipv6(sa6, "2001:db8:63b3:1::3490", ipv6_text, sizeof(ipv6_text));

    std::println("ipv4={}", ipv4_text);
    std::println("ipv6={}", ipv6_text);

    return util::tryCatch([&addr_info] {
        srv::init(addr_info);
        http::display_addr_info(addr_info);
    });
}
