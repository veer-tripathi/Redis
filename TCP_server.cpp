#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

struct in_addr{
    uint32_t s_addr; //32 bit IP address (IPv4 in big-endian format)
};


struct sockaddr_in{
    uint16_t sin_family; //address family
    uint16_t sin_port;   //port number in big-endian format
    struct in_addr sin_addr; //IP address
};


// struct sockaddr_in6 {
//     uint16_t        sin6_family;     // Address family → AF_INET6
//     uint16_t        sin6_port;       // Port (network byte order)
//     uint32_t        sin6_flowinfo;   // Flow label (usually ignored)
//     struct in6_addr sin6_addr;       // IPv6 address
//     uint32_t        sin6_scope_id;   // Scope (used for link-local)
// };

// struct in6_addr {
//     uint8_t         s6_addr[16];   // IPv6
// };

static void do_something(int connfd){
    char rbuf[64];
    ssize_t n=read(connfd, rbuf, sizeof(rbuf));
    if(n<0){
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[]="hello";
    write(connfd, wbuf, sizeof(wbuf));
}


int main(){

    //create socket
    //AF_INET -> IPv4 and AF_INET6 -> IPv6
    //SOCK_STREAM -> TCP and SOCK_DGRAM -> UDP
    //0 -> default protocol
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        die("socket()");
    }

    //this is needed for most server applications
    int val=1;
    //used to modify the behaviour of sockets
    //tells the kernal which settings we want to change, at what level, and the new value
    //-->SO_REUSEADDR
        //tells the kernel to reuse a local socket in TIME_WAIT state,
        //without waiting for its natural timeout to expire.

    //The effect of SO_REUSEADDR is important: if it’s not set to 1, 
    //a server program cannot bind to the same IP:port it was using after a restart

    //you should enable SO_REUSEADDR for all listening sockets
    
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    //bind the socket to an IP and PORT
    //we will bind to a wildcard address 0.0.0.0:1234
    struct sockaddr_in addr={};
    addr.sin_family = AF_INET; //IPv4
    addr.sin_port = htons(1234); //port 1234, htons converts to network byte order, 16-bit port number
    addr.sin_addr.s_addr = htonl(0); //wildcard address 0.0.0.0, htonl converts to network byte order, 32-bit address

    //struct sockaddr_in holds an IPv4:port pair stored as big-endian numbers, converted by htons() and htonl().
    // For example, 1.2.3.4 is represented by htonl(0x01020304).

    int rv=bind(socket_fd,(const struct sockaddr*)&addr, sizeof(addr));
    if(rv){
        die("bind()");
    }

    //All the previous steps are just passing parameters. The socket is actually created after listen().
    rv = listen(socket_fd, SOMAXCONN);//second parameter is size of the backlog queue, usualyy 4096 in linux
    if (rv) { die("listen()"); }

    while(true){
        //accept incoming connections
        struct sockaddr_in client_addr={};
        socklen_t addrlen=sizeof(client_addr);
        int connfd=accept(socket_fd, (struct sockaddr*)&client_addr, &addrlen);
        if(connfd<0){
            die("accept()");
        }

        //do_something(connfd);
        close(connfd);
    }
    return 0;

}