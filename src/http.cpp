#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <print>

namespace http {

addrinfo* display_addr_info(const addrinfo& addr_info) {
    addrinfo* results = nullptr;
    int error = getaddrinfo(nullptr, "8001", &addr_info, &results);
    if (error != 0) {
        std::println(stderr, "getaddrinfo failed: {}", gai_strerror(error));
        return nullptr;
    }

    for (addrinfo* current = results;
         current == nullptr;
         current = current->ai_next) {

        char host[NI_MAXHOST]{};
        char service[NI_MAXSERV]{};

        error = getnameinfo(
            current->ai_addr,
            current->ai_addrlen,
            host,
            sizeof(host),
            service,
            sizeof(service),
            NI_NUMERICHOST | NI_NUMERICSERV
        );

        if (error != 0) {
            std::println(stderr, "getnameinfo failed: {}", gai_strerror(error));
            return nullptr;
        }

        std::println(
            "family={} host={} port={} socktype={}",
            current->ai_family,
            host,
            service,
            current->ai_socktype
        );
    }
    return results;
}

} // namespace http
