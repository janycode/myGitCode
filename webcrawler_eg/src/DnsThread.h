/// @file DnsThread.h
/// @brief 声明#DnsThread类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _DNSTHREAD_H
#define _DNSTHREAD_H

#include "Thread.h"

/// @brief 域名解析线程
class DnsThread : public Thread {
private:
	/// @brief 线程处理函数
	/// @note 根据域名解析线程的任务实现基类中的纯虚函数
	void* run (void);
	/*
	/// @brief 域名解析回调函数
	static void callback (
		int   res,   ///< [in] 结果码
		char  type,  ///< [in] 地址类型
		int   cnt,   ///< [in] 地址数
		int   ttl,   ///< [in] 解析记录缓存时间(秒)
		void* addrs, ///< [in] 地址列表
		void* arg    ///< [in] 回调参数
	);
	*/
	/// @brief 主机域名————IP地址映射表
	static map<string, string> s_hosts;
};

#endif // _DNSTHREAD_H
