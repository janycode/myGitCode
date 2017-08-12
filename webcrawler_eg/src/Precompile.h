/// @file Precompile.h
/// @brief 预编译头文件
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _PRECOMPILE_H
#define _PRECOMPILE_H

// UNIX系统调用
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <pthread.h>
#include <signal.h>
#include <dlfcn.h>
#include <netdb.h>
#include <regex.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 标准C库
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// 标准C++库
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
using namespace std;
/*
// libevent库
// Ubuntu: apt-get install libevent-dev
#include <event.h>
#include <evdns.h>
*/
#endif // _PRECOMPILE_H
