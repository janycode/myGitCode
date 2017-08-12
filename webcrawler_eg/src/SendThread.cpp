/// @file SendThread.cpp
/// @brief 实现#SendThread类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"

// 线程处理函数
// 根据发送线程的任务实现基类中的纯虚函数
void* SendThread::run (void) {
	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"发送线程开始");

	// 无限循环
	for (;;)
		// 启动一个抓取任务
		g_app->startJob ();

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"接收线程终止");
	// 终止线程
	return NULL;
}
