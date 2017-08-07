/// @file SendThread.h
/// @brief 声明#SendThread类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _SENDTHREAD_H
#define _SENDTHREAD_H

#include "Thread.h"

class Socket;

/// @brief 发送线程
class SendThread : public Thread {
private:
	/// @brief 线程处理函数
	/// @note 根据发送线程的任务实现基类中的纯虚函数
	void* run (void);
};

#endif // _SENDTHREAD_H
