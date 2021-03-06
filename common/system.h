//
//  net.h
//  unix_net
//
//  Created by 张衡 on 15/10/5.
//
//

#ifndef net_h
#define net_h

#include <stdio.h>

/// 打开socket，获取socket的文件描述符
/// 返回socket文件描述符
int
Socket(int domain, int type, int protocol);

/// 将socket连接到指定地址
void
Connect(int socket, const struct sockaddr *address,
        socklen_t address_len);

#define TERM_SKIP 0 // 仅在缓冲区未满时填充0终结符
#define TERM_FILLED 1 // 在填充缓冲区的最后添加0终结符，方便直接输出

/// 读取socket内容，尽量填满传入的buf缓冲区
/// 返回读取的字节数，一定大于等于0，小于0会直接退出进程
/// 等于0时表示读取结束
ssize_t
Read(int fildes, void *buf, size_t nbyte);

/// 将缓冲区内容写入socket
ssize_t
Write(int fildes, const void *buf, size_t nbyte);

/// 将socket绑定到本地地址
void
Bind(int socket, const struct sockaddr *address, socklen_t address_len);

/// 开启socket监听
void
Listen(int socket, int backlog);

/// 接收socket请求，建立socket连接
int
Accept(int socket, struct sockaddr *restrict address,
       socklen_t *restrict address_len);

/// 获取对面socket的信息
void
Getsockname(int socket, struct sockaddr *restrict address,
            socklen_t *restrict address_len);

/// 获取对面socket的信息
void
Getpeername(int socket, struct sockaddr *restrict address,
            socklen_t *restrict address_len);

/// 关闭指定socket
void
Close(int fildes);

/// 以某种方式关闭己方的socket连接
void
Shutdown(int socket, int how);

/// fork进程
pid_t
Fork();

// 信号处理函数签名
typedef void signal_handler(int);

/// 设置信号处理
void
setup_signal(int signo, signal_handler handler);


#endif /* net_h */
