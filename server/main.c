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

const int BUFFER_LENGTH = 1024;
const int QUEUE_LEN = 10;

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

/// 服务器端处理函数
void
server_process(int socketfd){
    char buff[BUFFER_LENGTH];
    
    // 写入时间响应
    /* time_t ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    Write(socketfd, buff, strlen(buff));*/
    
    // 输出客户端的ip地址与端口号
    char addr_buffer[ADDR_PAIR_LEN];
    get_peer_socket_info(socketfd, addr_buffer, sizeof(addr_buffer));
    printf("Client is from %s\n", addr_buffer);
    
    
    // 读取客户端传来的信息
    while( Read(socketfd, buff, sizeof(buff)-1, TERM_FILLED) > 0 ){
        // echo回去
        Write(socketfd, buff, strlen(buff));
    }
    
    // 读到0个字节，表示客户端关闭，socket获取EOF状态，服务器处理结束
}

int main(int argc, char* argv[]){
    // 准备信号处理
    setup_signal(SIGCHLD, sigchild_handler);
    
    struct sockaddr_in socket_addr;
    bzero(&socket_addr, sizeof(socket_addr));

    socket_addr.sin_family      = AF_INET;
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_addr.sin_port        = htons(8124);
    
    // 创建socket
    int listen_fd = Socket(AF_INET, SOCK_STREAM, 0);
    // 绑定到本地地址
    Bind(listen_fd, (const struct sockaddr *) &socket_addr, sizeof(socket_addr));
        
    // 开始监听
    Listen(listen_fd, QUEUE_LEN);
    
    puts("Server launched");
    
    int connect_fd = 0;
    while(1){
        // 接收请求
        connect_fd = Accept(listen_fd, (struct sockaddr *)NULL, NULL);
        puts("Connected");
        
        pid_t child_id = 0;
        if( (child_id = Fork()) == 0 ){
            Close(listen_fd); // 子进程用不到监听socket，直接关掉，减少引用计数
            
            // 服务器端处理
            server_process(connect_fd);
            
            // 处理完关闭
            Close(connect_fd);
            
            puts("Connection closed");
            exit(0); // 子进程完成后退出，如果不加处理会造成子进程的僵尸状态
        }else{
            Close(connect_fd); // 新建的socket连接交给子进程处理，父进程这里直接关闭
        }
    }
}
