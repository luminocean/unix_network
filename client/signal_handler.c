//
//  signal_handler.c
//  unix_net
//
//  Created by 张衡 on 15/10/7.
//
//

#include "signal_handler.h"
#include "util.h"

void
sigpipe_handler(int signo){
    error("sigpipe error");
}