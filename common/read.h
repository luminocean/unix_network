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

ssize_t
read_line(int fd, void *buffer_ptr, size_t buffer_size);

#endif /* read_h */