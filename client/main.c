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
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "util.h"
#include "system.h"
#include "read.h"

const int BUFFER_SIZE = 1024;

/// 客户端处理行为
void
client_process(int socket_fd){
    char buffer[BUFFER_SIZE];
    
    // 读入标准输入流
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // 写入socket连接
        Write(socket_fd, buffer, strlen(buffer));
        
        // 读取服务器回传的socket数据
        int read_flag = 0; // 是否继续读行
        do{
            read_flag = read_line(socket_fd, buffer, sizeof(buffer));
            if( read_flag == READ_ERROR )
                error("read line error");
            if( read_flag == READ_EOF )
                error("server terminated");
            
            // 输出缓冲内容
            if (fputs(buffer, stdout) == EOF)
                error("fputs error");

        }while(read_flag == READ_CONTINUE); // 也就是说只有返回了READ_LINE才会正常离开循环
        
        // Read(socket_fd, buffer, sizeof(buffer), TERM_FILLED); // 原来只读一次的方案
    }
}

/// SIGPIPE信号处理
/// 当向RST状态的socket写入数据时发生
/// 用于处理服务器进程崩溃或者服务器重启后丢失原有会话的情形
void
sigpipe_handler(int signo){
    error("sigpipe error");
}

int
main(int argc, const char * argv[]) {
    // 设置信号处理
    setup_signal(SIGPIPE, sigpipe_handler);
    
    // 创建socket地址结构体
    struct sockaddr_in socket_addr; // internet风格的socket地址
    bzero(&socket_addr, sizeof(socket_addr)); // 清空结构体
    
    socket_addr.sin_family = AF_INET; // Address Family：网际协议，如TCP, UDP
    socket_addr.sin_port   = htons(8124); // 设置端口，host to network short (address)
    
    // 将文本的ip地址字符串转化成数字形式，然后设置到socket地址结构体里面的具体地址上
    if (inet_pton(AF_INET, argv[1], &socket_addr.sin_addr) <= 0)
        error("IP address error");
    
    /// 创建一个[网际][字节流]socket，得到一个文件描述符
    int socket_fd = Socket(AF_INET, SOCK_STREAM, 0);
    
    // 使用socket和地址建立tcp连接，之后套接字描述符就可以使用read读取
    Connect(socket_fd, (const struct sockaddr*)&socket_addr, sizeof(socket_addr));
    
    // 获取连接后本地socket的IP和端口信息
    char addr_buffer[ADDR_PAIR_LEN];
    get_local_socket_info(socket_fd, addr_buffer, sizeof(addr_buffer));
    
    puts("Connected to server");
    printf("Local address is %s\n", addr_buffer);
    
    /// 客户端处理 ///
    client_process(socket_fd);
    ///////////
    
    Close(socket_fd);

    getchar();
    
    return 0;
}
