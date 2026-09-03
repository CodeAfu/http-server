#include "http.hpp"
#include "server.hpp"
#include "utils.hpp"
#include <sys/socket.h>

int main() {
    http::AddrInfo addr_info = http::AddrInfo{
        .family = AF_INET,
        .socktype = SOCK_STREAM,
    };
    return util::tryCatch([&addr_info]{
        addr_info.display_addr_info();
        srv::init(addr_info); 
    });
}
