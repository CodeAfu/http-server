#include <string>

struct addrinfo;

namespace http {
addrinfo* get_addr_info(const char *host, const char *port, const ::addrinfo& hints);
void print_addr_info(const ::addrinfo& addr_info);

bool send_msg(int sock_fd, const std::string& msg, int flags);
const std::string& recv_msg(int sock_fd, std::string& msg, int flags);
} // namespace http
