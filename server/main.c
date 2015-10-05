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
#include "net.h"

int BUFFER_LENGTH = 1024;
int QUEUE_LEN = 10;

int main(int argc, char* argv[]){
    struct sockaddr_in socket_addr;
    bzero(&socket_addr, sizeof(socket_addr));

    socket_addr.sin_family      = AF_INET;
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_addr.sin_port        = htons(8124);
    
    // 创建socket
    int socket_fd = Socket(AF_INET, SOCK_STREAM, 0);
    // 绑定到本地地址
    Bind(socket_fd, (const struct sockaddr*) &socket_addr, sizeof(socket_addr));
    // 开始监听
    Listen(socket_fd, QUEUE_LEN);
    
    puts("Server launched");
    
    int connect_fd = 0;
    char buff[BUFFER_LENGTH];
    while(1){
        // 接收请求
        connect_fd = Accept(socket_fd, (struct sockaddr *)NULL, NULL);
        puts("Connected");
        
        // 写入时间相应
        time_t ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        Write(connect_fd, buff, strlen(buff));
        
        // 写入完毕，关掉socket
        Close(connect_fd);
        puts("Connection closed");
    }
}
