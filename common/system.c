//
//  net.c
//  unix_net
//
//  Created by 张衡 on 15/10/5.
//
//

#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <signal.h>
#include "system.h"
#include "util.h"

int
Socket(int domain, int type, int protocol){
    int socketfd = 0;
    if ( (socketfd = socket(domain, type, protocol)) < 0)
        error("socket error");
    
    return socketfd;
}

void
Connect(int socket, const struct sockaddr *address, socklen_t address_len){
    if (connect(socket, address, address_len) < 0)
        error("connect error");
}

ssize_t
Read(int fildes, void *buf, size_t nbyte, int terminate){
    if( terminate == TERM_FILLED )
        nbyte--;
    
    ssize_t n = read(fildes, buf, nbyte);
    if( n < 0 )
        error("read error");
    
    if( n > 0 )
        ((char *)buf)[n] = 0;
    
    return n;
}

ssize_t
Write(int fildes, const void *buf, size_t nbyte){
    ssize_t n= 0;
    if((n = write(fildes, buf, nbyte))<0)
        error("Writing failed");
    
    return n;
}

void
Bind(int socket, const struct sockaddr *address, socklen_t address_len){
    if(bind(socket, address, address_len) < 0)
        error("bind error");
}

void
Listen(int socket, int backlog){
    if(listen(socket, backlog) < 0)
        error("Listening failed");
}

int
Accept(int socket, struct sockaddr *restrict address,
       socklen_t *restrict address_len){
    
    int fd = 0;
    
    // 这是为了防御中断错误，如果发生则自动重来
    while(1){
        if( (fd = accept(socket, address, address_len)) < 0){
            if(errno == EINTR)
                continue;
            
            error("accept error");
        }
        
        return fd;
    }
}

void
Getsockname(int socket, struct sockaddr *restrict address,
            socklen_t *restrict address_len){
    if( getsockname(socket, address, address_len) < 0 )
        error("getsockname error");
}

void
Getpeername(int socket, struct sockaddr *restrict address,
            socklen_t *restrict address_len){
    if( getpeername(socket, address, address_len) < 0 )
        error("getpeername error");
}

void
Close(int fildes){
    if(close(fildes) < 0)
        error("close error");
}

pid_t
Fork(){
    pid_t child_id = fork();
    if(child_id < 0)
        error("fork error");
    
    return child_id;
}

void
setup_signal(int signo, signal_handler handler){
    // 初始化sigaction结构
    struct sigaction act;
    act.sa_handler= *handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    
    if( sigaction(signo, &act, NULL) < 0 )
        error("sigaction error");
}


