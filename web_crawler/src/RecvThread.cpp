/// @file RecvThread.cpp
/// @brief 实现#RecvThread类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "RecvThread.h"
#include "Socket.h"

// 构造器
RecvThread::RecvThread (
	Socket* socket // [输入] 套接字
	) : m_socket (socket) {} // 初始化套接字

// 析构器
RecvThread::~RecvThread (void) {
	// 销毁套接字
	delete m_socket;
}

// 线程处理函数
// 根据接收线程的任务实现基类中的纯虚函数
void* RecvThread::run (void) {
	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"接收线程开始");

	// 通过套接字接收超文本传输协议响
	// 应，根据其执行情况停止抓取任务
	g_app->stopJob (m_socket->recvResponse ());
	// 对象自毁
	delete this;

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"接收线程终止");
	// 终止线程
	return NULL;
}
