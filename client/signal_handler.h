//
//  signal_handler.h
//  unix_net
//
//  Created by 张衡 on 15/10/7.
//
//

#ifndef signal_handler_h
#define signal_handler_h

/// SIGPIPE信号处理
/// 当向RST状态的socket写入数据时发生
/// 用于处理服务器进程崩溃或者服务器重启后丢失原有会话的情形
void
sigpipe_handler(int signo);

#endif /* signal_handler_h */
