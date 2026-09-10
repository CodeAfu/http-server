#include "http.hpp"
#include "util.hpp"
#include "server.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <print>
#include <source_location>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>

// private
namespace srv {
template <typename F>
bool try_srv_func( // TODO: delete
    F&& func,
    std::source_location location = std::source_location::current()
);
void sigchld_handler(int s);
} // namespace srv

void srv::run(Server& srv) {
    sockaddr_storage client_addr;
    char s[INET6_ADDRSTRLEN];

    // cleanup zombie processes
    struct sigaction sa{};
    sa.sa_handler = srv::sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        int error = errno;
        std::println(stderr, "server: sigaction failed: {} (errno={})",
                     std::strerror(error),
                     error);
        exit(1);
    }

    // temporary printing
    util::devprint("server: waiting for connections");
     

    while (true) {
        int num_events = 
            ::poll(srv.pfds, Server::POLLFD_COUNT, Server::POLLFD_TIMEOUT);

        if (num_events == 0) {
            util::devprint("server: poll timed out ({}ms)", Server::POLLFD_TIMEOUT);
            continue;
        }

        if (num_events == -1)
            continue;

        // If not poll, ignore
        if (!(srv.pfds[0].revents & POLLIN))
            continue;

        int client_fd = srv::accept(srv, client_addr);
        if (client_fd == -1) 
            continue;
::
        inet_ntop(client_addr.ss_family,
                    http::get_in_addr((sockaddr *)&client_addr),
                    s,
                    sizeof(s));
        std::println("server: received connection from {}", s);

        const std::string msg = "Connected to genzoku's server!";
        const std::string payload = std::format(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: {}\r\n"
            "Connection: close\r\n"
            "\r\n{}",
            msg.size(), msg
        );

        pid_t f = fork();
        if (f == -1) {
            srv.error_no = errno;
            srv.error = std::strerror(srv.error_no);
            std::println(stderr, "server: error occured while forking: {} (errno={})",
                         srv.error,
                         srv.error_no);
            close(srv.sock_fd);
            _exit(0);
        } else if (f == 0) {
            close(srv.sock_fd);
            http::send_msg(client_fd, payload, 0);
            close(client_fd);
            _exit(0);
        } else {
            close(client_fd);
        }
    }
}

srv::Server srv::init() {
    // throw server_error("test");
    Server srv{};
    addrinfo hints = addrinfo{
        .ai_flags = AI_PASSIVE,     // fill the ip for me
        .ai_family = AF_INET,       // IPv4
        .ai_socktype = SOCK_STREAM, // TCP stream sockets
    };

    // initailize poll file descriptors
    for (std::size_t i = 0; i < Server::POLLFD_COUNT; i++) {
        srv.pfds[i].fd = -1;
        srv.pfds[i].events = POLLIN;
        srv.pfds[i].revents = 0;
    }


    srv.ai =
        http::get_addr_info(nullptr, std::to_string(Server::PORT).c_str(), hints);

    for (addrinfo *p = srv.ai; p != nullptr; p = p->ai_next)
        http::print_addr_info(*p);

    srv.sock_fd = ::socket(srv.ai->ai_family, srv.ai->ai_socktype,
                          srv.ai->ai_protocol);
    if (srv.sock_fd == -1) {
        srv.error_no = errno;
        srv.error = std::strerror(srv.error_no);
        std::println(stderr, "socket failed: {} (errno={})",
                     srv.error,
                     srv.error_no);
        return srv;
    }
    util::devprint("server: file descriptor created. srv_fd={}", srv.sock_fd);

    // ipv4 listener pollfd
    srv.pfds[0].fd = srv.sock_fd;
    util::devprint("server: poll_fd created. poll_fd={}", srv.pfds[0].fd);

    int yes = 1;
    int sock_opt = ::setsockopt(srv.sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)); // fix "Address already in use" error
    if (sock_opt == -1) {
        srv.error_no = errno;
        srv.error = std::strerror(srv.error_no);
        std::println(stderr, "server: setsockopt failed: {} (errno={})",
                     srv.error,
                     srv.error_no);
        return srv;
    }

    if (!srv::bind(srv))
        return srv;

    if (!srv::listen(srv))
        return srv;

    std::println("server: listening on port {}", Server::PORT);

    srv.success = true;
    return srv;
}


bool srv::bind(Server& s) {
    if (s.ai == nullptr) {
        s.error = "No address info avaiable";
        return false;
    }

    int res = ::bind(s.sock_fd, s.ai->ai_addr, s.ai->ai_addrlen);

    if (res == -1) {
        s.error_no = errno; 
        s.error = std::strerror(s.error_no);
        std::println(stderr, "bind failed: {} (errno={})",
                     s.error,
                     s.error_no);
        return false;
    }
    util::devprint("server: bind success. srv_fd={}", s.sock_fd);
    return true; 
}

bool srv::listen(Server& s) {
    int res = ::listen(s.sock_fd, Server::BACKLOG);
    if (res == -1) { 
        int error = errno;
        std::println(stderr, "listen failed: {} (errno={})",
                     std::strerror(error), error);
        return false;
    }
    util::devprint("server: listen success. srv_fd={}", s.sock_fd);
    return true;
}

bool srv::connect(int client_fd, const ::addrinfo *srv_addr) {
    int res = ::connect(client_fd, srv_addr->ai_addr, srv_addr->ai_addrlen);
    if (res == -1) {
        int error = errno;
        std::println(stderr, "connection failed: {} (errno{})",
                     std::strerror(error),
                     error);
        return false;
    }
    util::devprint("server: connection success. client_fd={}", client_fd);
    return true;
}

int srv::accept(Server& s, ::sockaddr_storage& client_addr) {
    socklen_t client_len = sizeof(client_addr);
    int client_fd = ::accept(s.sock_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
    if (client_fd == -1) {
        int error = errno;
        s.error = std::strerror(error);
        std::println(stderr, "connection failed: {} (errno={})",
                     std::strerror(error),
                     error);
        return -1;
    }
    util::devprint("server: connection accepted. client_fd={}", client_fd);
    return client_fd;
}

char* srv::parse(char line[], const char symbol[]) {
    char *copy = (char *)malloc(strlen(line) + 1);
    if (copy == NULL) 
        return NULL;

    strcpy(copy, line);

    strtok(copy, symbol);
    char *token = strtok(NULL, " ");
    const char *source;

    if (token == NULL)
        source = "";
    else 
        source = token;

    char *message= (char *)malloc(strlen(source) + 1);
    if (message != NULL) 
        strcpy(message, source);

    free(copy);

    return message;
}

srv::Server::~Server() {
    util::devprint("Server destructor called");
    if (sock_fd != -1)
        ::close(sock_fd);
    if (ai != nullptr) 
        freeaddrinfo(ai); // free the linked list
}

// private
namespace srv {
void sigchld_handler(int s) {
    (void)s;
    int saved_errno = errno;
    while(::waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

template <typename F>
bool try_srv_func( // TODO: delete
    F&& func,
    std::source_location location
) {
    int result = func();
    if (result == -1) {
        int error = errno;
        std::println(stderr, "{}:{} failed, {} (errno{}) [{}]",
                     location.file_name(), location.line(),
                     std::strerror(error), error, location.function_name());
        return false;
    }
    return result != 1;
}
} // namespace srv

