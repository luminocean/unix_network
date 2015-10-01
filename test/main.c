//
//  main.c
//  test
//
//  Created by 张衡 on 15/10/1.
//
//

#include <stdio.h>
#include "read.h"
#include "util.h"

char buffer[READ_BUFFER_SIZE];

int main(int argc, const char * argv[]) {
    ssize_t result_count = 0;
    
    while(1){
        if( (result_count = read_line(0, buffer, sizeof(buffer))) < 0 ){
            error("read line error");
        }
        
        printf("Read line: %s", buffer);
    }
    
    return 0;
}
