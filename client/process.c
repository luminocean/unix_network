//
//  process.c
//  unix_net
//
//  Created by 张衡 on 15/10/7.
//
//

#include <sys/select.h>
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
    
    int limit_fd = 0; // 监控的fd集合中最大的fd+1
    while(1){
        FD_SET(src_fd, &fdset); // 登记要监控的文件描述符
        FD_SET(socket_fd, &fdset);
        
        limit_fd = MAX(src_fd, socket_fd) + 1;
        
        // 进入select等待，直到标准输入流或者socket可读才解除阻塞状态
        // 之所以要用select是因为如果客户端阻塞在标准输入流的时候对面服务器关闭，
        // 服务器传来的fin报文进程是没办法第一时间知道的，而是要等到接触stdin阻塞且往socket里面写的时候才会得到RST响应，这就有点晚了，因此使用select可以及时获知对面服务器关闭的信息
        select(limit_fd, &fdset, NULL, NULL, NULL);
        
        // 源fd可读
        if( FD_ISSET(src_fd, &fdset) ){
            if(fgets(buffer, sizeof(buffer), stdin) == NULL)
                return; // 如果源fd读取返回NULL，表示已经读完，客户端主要处理结束
            
            // 写入socket连接
            Write(socket_fd, buffer, strlen(buffer));
        }
        
        // socket可读
        if( FD_ISSET(socket_fd, &fdset) ){
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
}

void
client_process(int socket_fd){
    talk_to_server(fileno(stdin), socket_fd);
}