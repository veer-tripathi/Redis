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


int main(){

    //create socket
    //AF_INET -> IPv4 and AF_INET6 -> IPv6
    //SOCK_STREAM -> TCP and SOCK_DGRAM -> UDP
    //0 -> default protocol
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        die("socket()");
    }

    //bind the socket to an IP and PORT
    //we will bind to a wildcard address 0.0.0.0:1234
    struct sockaddr_in addr={};
    addr.sin_family = AF_INET; //IPv4
    addr.sin_port = ntohs(1234); //ntohs() converts a 16-bit number from network byte order to host byte order.
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); //ntohl(INADDR_LOOPBACK) converts the loopback IP (127.0.0.1) from network byte order into host byte order for local use.
    //struct sockaddr_in holds an IPv4:port pair stored as big-endian numbers, converted by htons() and htonl().
    // For example, 1.2.3.4 is represented by htonl(0x01020304).

    int rv=connect(socket_fd,(const struct sockaddr*)&addr, sizeof(addr));
    if(rv){
        die("connect()");
    }

    // Message that will be sent to the server
char msg[] = "hello";

/*
 * write():
 * Sends data to the file descriptor `fd`
 * - fd      : socket connected to the server
 * - msg     : pointer to data to send
 * - strlen(msg) : number of bytes to send (without '\0')
 */
write(socket_fd, msg, strlen(msg));


// Buffer to store data received from the server
// Initialized to zero so it is null-terminated by default
char rbuf[64] = {0};

/*
 * read():
 * Reads data from the socket into rbuf
 * - fd                : socket descriptor
 * - rbuf              : buffer to store received data
 * - sizeof(rbuf) - 1  : leave space for '\0'
 *
 * Returns:
 *  - number of bytes read (>= 0)
 *  - -1 on error
 */
ssize_t n = read(socket_fd, rbuf, sizeof(rbuf) - 1);

if (n < 0) {
    // If read() fails, print error and abort program
    die("read");
}

/*
 * Since rbuf is null-terminated, it can be safely printed as a string
 */
printf("server says: %s\n", rbuf);

/*
 * close():
 * Closes the socket
 * Releases kernel resources
 */
close(socket_fd);

// Program finished successfully
return 0;


}