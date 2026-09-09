#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT "8096"
#define MAXDATASIZE 1000

void* get_in_addr(struct sockaddr *sa);

int main(int argc, char* argv[]) {
    int sockfd;
    int numbytes;
    struct addrinfo hints;
    struct addrinfo *srv_info, *p;
    char buf[MAXDATASIZE];
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (argc != 2) {
        printf("usage: client hostname\n");
        exit(1);
    }

    rv = getaddrinfo(argv[1], PORT, &hints, &srv_info);
    if (rv != 0) {
        perror("getaddrinfo failed: %s"), gai_strerror(rv);
        exit(1);
    }

    for (p = srv_info; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("client: socket");
            continue;
        }

        inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof(s));
        printf("client: attempting connection to %s\n", s);

        int conn = connect(sockfd, p->ai_addr, p->ai_addrlen);
        if (conn == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof(s));
    printf("client: connected to %s\n", s);

    freeaddrinfo(srv_info);

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    printf("client: received message=%s\n", buf);

    close(sockfd);
    return 0;
}

void* get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) 
        return &(((struct sockaddr_in*)sa)->sin_addr);
    else if (sa->sa_family == AF_INET6)
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    else
        return NULL;
}

