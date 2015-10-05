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

#define TERM_SKIP 0 // 仅在缓冲区未满时填充0终结符
#define TERM_FILLED 1 // 在填充缓冲区的最后添加0终结符，方便直接输出


/// 打开socket，获取socket的文件描述符
/// 返回socket文件描述符
int
Socket(int domain, int type, int protocol);

/// 将socket连接到指定地址
void
Connect(int socket, const struct sockaddr *address,
        socklen_t address_len);

/// 读取socket内容，尽量填满传入的buf缓冲区
/// 可以通过terminate参数选择要不要给填充过的缓冲区结尾添加0终结符
/// 返回读取的字节数，一定大于等于0，小于0直接退出进程
/// 等于0时表示读取结束
ssize_t
Read(int fildes, void *buf, size_t nbyte, int terminate);

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

/// 关闭指定socket
void
Close(int fildes);

/// fork进程
pid_t
Fork();


#endif /* net_h */
