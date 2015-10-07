//
//  process.c
//  unix_net
//
//  Created by 张衡 on 15/10/7.
//
//

#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include "process.h"
#include "system.h"
#include "read.h"
#include "util.h"

#define MAX(a,b) ( (a) > (b) ? (a) : (b) )

const int BUFFER_SIZE = 1024;

/// 与服务器进行交互
/// 从来源文件（文件描述符形式）中读取内容
/// 写入到目标socket后再读取socket内的响应内容
static void
talk_to_server(int src_fd, int socket_fd){
    // select要用的文件描述符集合
    fd_set fdset;
    FD_ZERO(&fdset);
    
    ////// 之后要用到的各种读取使用的buffer
    char buffer[BUFFER_SIZE];
    //////
    
    int stdin_eof = 0; // 标准输入是否已经输入完毕
    
    int limit_fd = 0; // 监控的fd集合中最大的fd+1
    while(1){
        FD_SET(src_fd, &fdset); // 登记要监控的文件描述符
        FD_SET(socket_fd, &fdset);
        
        limit_fd = MAX(src_fd, socket_fd) + 1;
        
        // 进入select等待，直到标准输入流或者socket可读才解除阻塞状态
        // 之所以要用select是因为如果客户端阻塞在标准输入流的时候对面服务器关闭，
        // 服务器传来的fin报文进程是没办法第一时间知道的，而是要等到接触stdin阻塞且往socket里面写的时候才会得到RST响应，这就有点晚了，因此使用select可以及时获知对面服务器关闭的信息
        select(limit_fd, &fdset, NULL, NULL, NULL);
        
        ssize_t n;
        // 源fd可读
        if( FD_ISSET(src_fd, &fdset) ){
            // 读到EOF
            if( (n = Read(src_fd, buffer, sizeof(buffer))) == 0 ){
                stdin_eof = 1;
                Shutdown(socket_fd, SHUT_WR); // stdin已经读完了，也就没有东西可以往socket里面写了
                FD_CLR(src_fd, &fdset); // 不再监听stdin
                continue;
            }
            
            // 写入socket连接，注意此时读取并写入的内容里包括换行符
            Write(socket_fd, buffer, n);
        }
        
        // socket可读
        if( FD_ISSET(socket_fd, &fdset) ){
            // 服务器端fin
            if( (n = Read(socket_fd, buffer, sizeof(buffer))) == 0 ){
                if( stdin_eof )
                    return; // 我方终止在前，正常退出
                error("Server side terminated unexpectly"); // 否则说明是服务器端自行退出了
            }
            
            Write(fileno(stdout), buffer, n);
        }
    }
}

void
client_process(int socket_fd){
    talk_to_server(fileno(stdin), socket_fd);
}