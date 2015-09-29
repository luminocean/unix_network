//
//  main.c
//  server
//
//  Created by 张衡 on 15/9/29.
//
//

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "util.h"

int BUFFER_LENGTH = 1024;

int main(int argc, char* argv[]){
    struct sockaddr_in socket_addr;
    bzero(&socket_addr, sizeof(socket_addr));

    socket_addr.sin_family      = AF_INET;
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_addr.sin_port        = htons(8124);
    
    // 创建socket
    int socket_fd = 0;
    if ( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("socket error");

    if(bind(socket_fd, (const struct sockaddr*) &socket_addr, sizeof(socket_addr)) < 0)
        error("Binding failed");
    
    if(listen(socket_fd, 10) < 0)
        error("Listening failed");
    
    puts("Server launched");
    
    int connect_fd = 0;
    char buff[BUFFER_LENGTH];
    while(1){
        // 接收请求
        connect_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL);
        
        puts("Connected");
        
        time_t ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        
        ssize_t n= 0;
        if((n = write(connect_fd, buff, strlen(buff)))<0)
           error("Writing failed");
        
        if(close(connect_fd) < 0)
            error("Close failed");
        
        puts("Connection closed");
    }
}
