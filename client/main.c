//
//  main.c
//  unix_net
//
//  Created by 张衡 on 15/9/29.
//  Copyright © 2015年 LUMINOCEAN. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "util.h"
#include "system.h"


int CHUNK_SIZE = 64;
int READ_BUFFER_SIZE = 1024;

// socket读取缓冲
char buffer[1024+1];

int main(int argc, const char * argv[]) {
    /// 创建socket地址结构体
    
    struct sockaddr_in socket_addr; // internet风格的socket地址
    bzero(&socket_addr, sizeof(socket_addr)); // 清空结构体
    
    socket_addr.sin_family = AF_INET; // Address Family：网际协议，如TCP, UDP
    socket_addr.sin_port   = htons(8124); // 设置端口，host to network short (address)
    
    // 将文本的ip地址字符串转化成数字形式，然后设置到socket地址结构体里面的具体地址上
    if (inet_pton(AF_INET, argv[1], &socket_addr.sin_addr) <= 0)
        error("IP address error");
    
    
    /// 创建一个[网际][字节流]socket，得到一个文件描述符
    int socketfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    // 使用socket和地址建立tcp连接，之后套接字描述符就可以使用read读取
    Connect(socketfd, (const struct sockaddr*)&socket_addr, sizeof(socket_addr));
    
    puts("Connected to server");
    
    // 使用socket读取
    ssize_t n = 0;
    while ( (n = Read(socketfd, buffer, CHUNK_SIZE, TERM_FILLED)) > 0) {
        if (fputs(buffer, stdout) == EOF) // 输出缓冲内容
            error("fputs error");
    }
    
    Close(socketfd);

    getchar();
    
    return 0;
}
