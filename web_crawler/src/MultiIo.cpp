/// @file MultiIo.cpp
/// @brief 实现#MultiIo类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"

// 构造器
MultiIo::MultiIo (void) {
	// 创建epoll对象，若失败
	if ((m_epoll = epoll_create1 (0)) == -1)
		// 记录一般错误日志
		g_app->m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
			"epoll_create1: %s", strerror (errno));
}

// 析构器
MultiIo::~MultiIo (void) {
	// 销毁epoll对象
	close (m_epoll);
}

// 增加需要被关注的输入输出事件
// 成功返回true，失败返回false
bool MultiIo::add (
	int          fd,   // [输入] 发生输入输出事件的文件描述符
	epoll_event& event // [输入] 事件描述结构
	) const {
	// 将文件描述符及其被关注的事件加入epoll，若失败
	if (epoll_ctl (m_epoll, EPOLL_CTL_ADD, fd, &event) == -1) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"epoll_ctl: %s", strerror (errno));
		// 返回失败
		return false;
	}

	// 返回成功
	return true;
}

// 删除需要被关注的输入输出事件
// 成功返回true，失败返回false
bool MultiIo::del (
	int          fd,   // [输入] 发生输入输出事件的文件描述符
	epoll_event& event // [输入] 事件描述结构
	) const {
	// 将文件描述符及其被关注的事件从epoll中删除，若失败
	if (epoll_ctl (m_epoll, EPOLL_CTL_DEL, fd, &event) == -1) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"epoll_ctl: %s", strerror (errno));
		// 返回失败
		return false;
	}

	// 返回成功
	return true;
}

// 等待所关注输入输出事件的发生
// 返回处于就绪状态的文件描述符数，超时返回0，失败返回-1
int MultiIo::wait (
	epoll_event events[], ///< [输出] 事件描述结构数组
	int         max,      ///< [输入] 事件描述结构数组容量
	int         timeout   ///< [输入] 超时毫秒数，0立即超时，-1无限超时
	) const {
	// 等待epoll中的文件描述符发生所关注的事件
	int fds = epoll_wait (m_epoll, events, max, timeout);
	// 若发生除被信号中断以外的错误
	if (fds == -1 && errno != EINTR)
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"epoll_wait: %s", strerror (errno));

	// 返回处于就绪状态的文件描述符数，超时返回0，失败返回-1
	return fds;
}
