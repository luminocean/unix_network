//
//  read.h
//  unix_net
//
//  Created by 张衡 on 15/10/1.
//
//

#ifndef read_h
#define read_h

#include <stdio.h>

const static int READ_BUFFER_SIZE = 64;

/// 读取一行
/// 如果一行的长度过长，那么仅填满传入的空间，以0结尾
/// 返回0表示读取了完整的一行，返回1表示仅填满了缓冲区，还没有读到换行符
int
read_line(int fd, void *buffer_ptr, size_t buffer_size);

#endif /* read_h */