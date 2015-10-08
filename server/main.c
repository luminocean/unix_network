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
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "util.h"
#include "system.h"
#include "process.h"

const static int BUFFER_LENGTH = 1024;
const static int QUEUE_LEN = 10;

/// 子进程退出信号处理
void
sigchild_handler(int signo){
    pid_t pid;
    int stat;
    
    // 处理所有子进程的僵尸进程，避免遗漏
    while( (pid = waitpid(-1, &stat, WNOHANG)) > 0 ){
        printf("Child %d terminated\n", pid);
        fflush(stdout);
    }
    
    return;
}

// 输出客户端的ip地址与端口号
void
print_peer_info(int socketfd){
    char addr_buffer[ADDR_PAIR_LEN];
    get_peer_socket_info(socketfd, addr_buffer, sizeof(addr_buffer));
    printf("Client is from %s\n", addr_buffer);
}


/// 建立与客户端的socket连接
/// 并在后续与客户端进行交互
void
handle_clients(int listen_fd){
    fd_set fdset; // 要监听的fd集合
    FD_ZERO(&fdset);
    FD_SET(listen_fd, &fdset); // 首先开始监控listen_fd
    
    int max_fd = listen_fd; // 目前最大的fd就是listen_fd
    int i; // 循环计数
    
    // 存放所有客户端连接描述符的数组
    // 要添加新的描述符时，选择索引最小的非-1位置填充
    int client_fds[FD_SETSIZE];
    int max_i = -1; // 目前最大的索引值
    for(i=0; i<FD_SETSIZE; i++){
        client_fds[i] = -1; // 默认使用-1占位
    }
    
    while(1){
        fd_set set = fdset; // 每次调用select以后都会让集合清空，因此需要重新赋值
        select(max_fd+1, &set, NULL, NULL, NULL); // 开始监控
        
        // 有新客户端连接
        if( FD_ISSET(listen_fd, &set) ){
            int client_fd = 0;
            
            // 接收请求
            client_fd = Accept(listen_fd, (struct sockaddr *)NULL, NULL);
            puts("Connected");
            print_peer_info(client_fd);
            
            // 开始监控
            FD_SET(client_fd, &fdset);
            
            // 将新的连接描述符加到客户连接数组里
            for(i=0; i<FD_SETSIZE; i++){
                if( client_fds[i] == -1 ){
                    client_fds[i] = client_fd;
                    break;
                }
            }
            if( i == FD_SETSIZE )
                error("Too many clients");
            
            // 更新监听最大描述符号
            if( client_fd > max_fd )
                max_fd = client_fd;
            // 更新目前客户连接数组最大索引值，这个值后面循环检查客户端连接状态时要用
            if( i > max_i )
                max_i = i;
        }
        
        int client_fd;
        // 遍历所有客户端连接
        for( i=0; i<=max_i; i++ ){
            if( (client_fd = client_fds[i]) == -1 )
                continue; // 数组内可能有-1的空档，这是由于原来的连接关闭后被重置成-1造成的，直接跳过
            
            // 读取socket使用的缓冲区
            char buffer[BUFFER_LENGTH];
            ssize_t n;
            if(FD_ISSET(client_fd, &set)){ // 某个客户端连接可以读取
                // 客户端方面关闭了写socket流
                if( (n = Read(client_fd, buffer, sizeof(buffer))) == 0 ){
                    Close(client_fd); // 我方直接关闭socket
                    FD_CLR(client_fd, &fdset);
                    client_fds[i] = -1; // 关闭的client_fd对应的描述符重置成-1
                    
                    puts("Connection closed");
                    fflush(stdout);
                }else{
                    // 正常情况下，直接写回socket
                    Write(client_fd, buffer, n);
                }
            }
        }
    }
}

int main(int argc, char* argv[]){
    // 准备信号处理
    setup_signal(SIGCHLD, sigchild_handler);
    
    // 初始化服务器端要监听的socket信息
    struct sockaddr_in socket_addr;
    bzero(&socket_addr, sizeof(socket_addr));
    socket_addr.sin_family      = AF_INET;
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_addr.sin_port        = htons(8124);
    
    // 创建socket
    int listen_fd = Socket(AF_INET, SOCK_STREAM, 0);
    
    // 绑定到本地地址，开始监听
    Bind(listen_fd, (const struct sockaddr *) &socket_addr, sizeof(socket_addr));
    Listen(listen_fd, QUEUE_LEN);
    puts("Server launched");
    
    // 与客户端交互
    handle_clients(listen_fd);
}
