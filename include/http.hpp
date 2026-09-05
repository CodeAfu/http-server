
struct addrinfo;

namespace http {
addrinfo* get_addr_info(const char *host, const char *port, const ::addrinfo& hints);
void print_addr_info(const ::addrinfo& addr_info);
}
