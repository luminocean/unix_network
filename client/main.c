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
    
    int socketfd = 0;
    if ( (socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("socket error");
    
    
    // 使用socket和地址建立tcp连接，之后套接字描述符就可以使用read读取
    if (connect(socketfd, (const struct sockaddr*)&socket_addr, sizeof(socket_addr)) < 0)
        error("Connect error");
    
    puts("Connected to server");
    // 使用socket读取
    ssize_t n = 0;
    while ( (n = read(socketfd, buffer, CHUNK_SIZE)) > 0) {
        buffer[n] = 0;	// c-style字符串结尾符
        if (fputs(buffer, stdout) == EOF) // 输出缓冲内容
            error("fputs error");
    }
    
    if (n < 0)
        error("read error");
    
    if(close(socketfd) < 0)
        error("close error");

    getchar();
    
    return 0;
}
