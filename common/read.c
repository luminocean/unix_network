//
//  read.c
//  unix_net
//
//  Created by 张衡 on 15/10/1.
//
//

#include <unistd.h>
#include <errno.h>
#include "read.h"

/// 从文件中读取内容，填满输入缓冲区（如果可能的话）
// 然后返回每次一个字节，也就是说多次调用这个方法可能只有一次是实际读取的，其他都是从缓冲区直接返回的
// 从而一次来提升读取效率
static ssize_t
buffered_read(int fd, char *character)
{
    static ssize_t read_count = 0; // 缓冲区内字符的数量
    static char read_buffer[READ_BUFFER_SIZE]; // 缓冲区
    static char *read_ptr = 0; // 当前正在读取的缓冲区指针
    
    // 如果缓冲区内的内容已经用完，则再次读取
    if(read_count <= 0){
        // 尝试读取数据
        while(1){
            if((read_count = read(fd, read_buffer, sizeof(read_buffer))) < 0){
                if( errno == EINTR ) continue;
                return -1;
            }else if(read_count == 0){
                // 已经读完
                return 0;
            }else{
                // 重置读取指针
                read_ptr = read_buffer;
                break;
            }
        }
    }
    
    // 缓冲区已经有内容，直接返回下一个字符
    read_count--;
    *character = *read_ptr++;
    return 1; // 表示读取了一个字节
}


int
read_line(int fd, void *buffer_ptr, size_t buffer_size)
{
    char character = 0; // 每次读取的单个字符
    ssize_t char_count = 0;
    char *write_ptr = buffer_ptr; // 写入缓冲区的指针
    
    int i = 0;
    for(i=0; i<buffer_size; i++){
        if( (char_count = buffered_read(fd, &character)) == 1 ){
            *write_ptr = character;
            write_ptr++;
            
            // 读到换行符，结束此次行读取
            if( character == '\n' ){
                return 0;
            }
            
        }else if ( char_count == 0 ){
            // 整个文件已经读完了，返回已经读入的字节数
            return i;
        }else{
            // 读取错误
            return -1;
        }
    }
    
    *write_ptr = 0;
    return 1; // 返回1表示还没有读到换行符，下次还要接着读取
}