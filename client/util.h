//
//  util.h
//  unix_net
//
//  Created by 张衡 on 15/9/29.
//  Copyright © 2015年 LUMINOCEAN. All rights reserved.
//

#ifndef util_h
#define util_h

#include <stdio.h>
#include <stdlib.h>

#define ADDR_PAIR_LEN 3*4+3+1+5+1 // IP:PORT/0长度

/// 打印出错误信息，然后退出进程
void error(const char *str);

/// 获取本地socket的地址信息，包括ip地址与端口
void get_local_socket_info(int socket_fd, char* buffer, int buffer_len);

/// 获取对向socket的地址信息，包括ip地址与端口
void get_peer_socket_info(int socket_fd, char* buffer, int buffer_len);

#endif /* util_h */
