#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string>
#include <vector>


static void msg(const char* msg){
    fprintf(stderr,"%s\n", msg);
}

static void die(const char* msg){
    int err=errno;
    fprintf(stderr,"[%d]%s\n",err,msg);
    abort();
}

//buf->current position in your memory where the data should be read into
static int32_t read_full(int fd, uint8_t*buf, size_t n){
    while(n){
        ssize_t rv=read(fd,buf,n);
        if(rv<=0){
            return -1;
        }
        assert((size_t)rv<=n);
        n-=(size_t)rv;
        buf+=rv;
    }
    return 0;
}

static int32_t write_all(int fd,uint8_t *buf, size_t n){
    while(n){
        ssize_t rv=write(fd,buf,n);
        if(rv<=0){
            return -1;
        }
        assert((size_t)rv<=n);
        n-=(size_t)rv;
        buf+=rv;
    }
    return 0;
}

static void append(std::vector<uint8_t>&buf,const uint8_t* data,size_t len){
    buf.insert(buf.end(),data,data+len);
}

const size_t max_size=32<<20;

static int32_t send_req(int fd,const uint8_t *text,size_t len){
    if(len>max_size){
        msg("send_req: len>max_size");
        return -1;
    }
    std::vector<uint8_t>buf;
    append(buf,(const uint8_t*)&len,4);
    append(buf,text,len);
    return write_all(fd,buf.data(),buf.size());
}

static int32_t read_res(int fd) {
    uint32_t len = 0;
    // 1. Read the 4-byte header
    int32_t err = read_full(fd, (uint8_t *)&len, 4);
    if (err) {
        msg(errno == 0 ? "EOF" : "read error");
        return err;
    }

    if (len > max_size) {
        msg("too long");
        return -1;
    }

    // 2. Read the body into a buffer large enough to hold 'len'
    std::vector<uint8_t> body(len);
    err = read_full(fd, &body[0], len);
    if (err) {
        msg("read body error");
        return err;
    }

    // 3. Print the result
    printf("len:%u data:%.*s\n", len, len < 100 ? len : 100, body.data());
    return 0;
}

int main(){
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd<0){
        die("socket");
    }
    struct sockaddr_in addr={};
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ntohl(INADDR_LOOPBACK);
    addr.sin_port=ntohs(1234);
    int rv=connect(fd,(struct sockaddr*)&addr, sizeof(addr));
    if(rv<0){
        die("connect");
    }

    // multiple pipelined requests
    std::vector<std::string> query_list = {
        "hello1", "hello2", "hello3",
        // a large message requires multiple event loop iterations
        std::string(max_size, 'z'),
        "hello5",
    };

    for (const std::string &s : query_list) {
        int32_t err = send_req(fd, (uint8_t *)s.data(), s.size());
        if (err) {
            goto L_DONE;
        }
    }

    for (size_t i = 0; i < query_list.size(); ++i) {
        int32_t err = read_res(fd);
        if (err) {
            goto L_DONE;
        }
    }
    L_DONE:
    close(fd);
    return 0;
}