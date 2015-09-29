//
//  util.c
//  unix_net
//
//  Created by 张衡 on 15/9/29.
//  Copyright © 2015年 LUMINOCEAN. All rights reserved.
//

#include "util.h"

void error(const char *str)
{
    fprintf(stderr, "%s\n", str);
    exit(1);
}