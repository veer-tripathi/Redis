#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>


// Print errno and message, then abort the program
static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int main() {

    /*
     * Create a socket
     * AF_INET      -> IPv4
     * SOCK_STREAM  -> TCP
     * 0            -> default protocol (TCP for SOCK_STREAM)
     */
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        die("socket()");
    }

    /*
     * Prepare the server address (IPv4)
     * sockaddr_in represents an IPv4 address + port
     */
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;        // Address family: IPv4

    /*
     * Port number must be in NETWORK byte order
     * htons() = host-to-network short (16-bit)
     */
    addr.sin_port = htons(1234);

    /*
     * INADDR_LOOPBACK represents 127.0.0.1
     * It is already defined in NETWORK byte order,
     * so it can be assigned directly.
     */
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    /*
     * Connect the socket to the server address
     * sockaddr_in is cast to sockaddr because connect()
     * accepts a generic sockaddr pointer.
     */
    int rv = connect(socket_fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv < 0) {
        die("connect()");
    }

    /*
     * Message to send to the server
     */
    char msg[] = "hello";

    /*
     * write():
     * Sends raw bytes to the connected socket
     * - socket_fd   : connected TCP socket
     * - msg         : buffer containing data
     * - strlen(msg) : number of bytes to send (no '\0')
     */
    write(socket_fd, msg, strlen(msg));

    /*
     * Buffer to receive server response
     * Initialized to zero so it is null-terminated
     */
    char rbuf[64] = {0};

    /*
     * read():
     * Reads data from the socket
     * - socket_fd        : socket descriptor
     * - rbuf             : destination buffer
     * - sizeof(rbuf) - 1 : leave space for '\0'
     *
     * Returns:
     *  - number of bytes read (>= 0)
     *  - -1 on error
     */
    ssize_t n = read(socket_fd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        die("read");
    }

    /*
     * rbuf is null-terminated, so it can be printed safely
     */
    printf("server says: %s\n", rbuf);

    /*
     * Close the socket and release kernel resources
     */
    close(socket_fd);

    return 0;
}
