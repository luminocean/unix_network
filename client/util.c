//
//  util.c
//  unix_net
//
//  Created by 张衡 on 15/9/29.
//  Copyright © 2015年 LUMINOCEAN. All rights reserved.
//

#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "util.h"
#include "system.h"

typedef int (*GETSOCKETNAME) (int, struct sockaddr *restrict, socklen_t *restrict);

void error(const char *str)
{
    fprintf(stderr, "%s\n", str);
    exit(1);
}

/// 通用socket打印函数
void get_socket_info(int socket_fd, GETSOCKETNAME func, char* buffer, int buffer_len){
    // 输出客户端的ip地址与端口号
    struct sockaddr_in sock;
    bzero(&sock, sizeof(sock));
    socklen_t len = sizeof(sock);
    
    char addr[3*4+3+1];
    func(socket_fd, (struct sockaddr *) &sock, &len);
    inet_ntop(AF_INET, &sock.sin_addr, (char *)&addr, sizeof(sock));
    snprintf(buffer, buffer_len, "%s:%d", addr, ntohs(sock.sin_port));
}

void get_local_socket_info(int socket_fd, char* buffer, int buffer_len){
    get_socket_info(socket_fd, getsockname, buffer, buffer_len);
}

void get_peer_socket_info(int socket_fd, char* buffer, int buffer_len){
    get_socket_info(socket_fd, getpeername, buffer, buffer_len);
}