/// @file UrlQueues.h
/// @brief 声明#UrlQueues类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _URLQUEUES_H
#define _URLQUEUES_H

#include "Url.h"

class UrlFilter;

/// @brief 统一资源定位符队列
class UrlQueues {
public:
	/// @brief 构造器
	UrlQueues (
		UrlFilter& filter ///< [in] 统一资源定位符过滤器
	);
	/// @brief 析构器
	~UrlQueues (void);

	/// @brief 压入原始统一资源定位符
	void pushRawUrl (
		RawUrl const& rawUrl ///< [in] 原始统一资源定位符
	);
	/// @brief 弹出原始统一资源定位符
	/// @return 原始统一资源定位符
	RawUrl popRawUrl (void);
	/// @brief 获取原始统一资源定位符数
	/// @return 原始统一资源定位符数
	size_t sizeRawUrl (void) const;
	/// @brief 原始统一资源定位符队列空否
	/// @retval true  空
	/// @retval false 不空
	bool emptyRawUrl (void) const;
	/// @brief 原始统一资源定位符队列满否
	/// @retval true  满
	/// @retval false 不满
	bool fullRawUrl (void) const;
	/// @brief 清空原始统一资源定位符队列
	void clearRawUrl (void);

	/// @brief 压入解析统一资源定位符
	void pushDnsUrl (
		DnsUrl const& dnsUrl ///< [in] 解析统一资源定位符
	);
	/// @brief 弹出解析统一资源定位符
	/// @return 解析统一资源定位符
	DnsUrl popDnsUrl (void);
	/// @brief 获取解析统一资源定位符数
	/// @return 解析统一资源定位符数
	size_t sizeDnsUrl (void) const;
	/// @brief 解析统一资源定位符队列空否
	/// @retval true  空
	/// @retval false 不空
	bool emptyDnsUrl (void) const;
	/// @brief 解析统一资源定位符队列满否
	/// @retval true  满
	/// @retval false 不满
	bool fullDnsUrl (void) const;
	/// @brief 清空解析统一资源定位符队列
	void clearDnsUrl (void);

	/// @brief 从超文本标记语言页面内容中抽取统一资源定位符
	void extractUrl (
		char const*   html,  ///< [in] 超文本标记语言页面内容字符串
		DnsUrl const& dnsUrl ///< [in] 被抽取页面解析统一资源定位符
	);

private:
	/// @brief 判断某统一资源定位符所表示的资源是否是二进制资源
	/// @retval true  是二进制资源
	/// @retval false 非二进制资源
	static bool isBinary (
		string const& strUrl ///< [in] 统一资源定位符字符串
	);
	/*
	/// @brief 将以毫秒表示的相对时间换算为以秒和纳秒表示的绝对时间
	/// @return 以秒和纳秒表示的绝对时间
	static timespec getTimespec (
		long msec ///< [in] 以毫秒表示的相对时间
	);
	*/
	/// @brief 统一资源定位符过滤器
	UrlFilter& m_filter;

	/// @brief 原始统一资源定位符队列
	list<RawUrl> m_rawUrlQueue;
	/// @brief 原始统一资源定位符队列互斥锁
	mutable pthread_mutex_t m_rawUrlMutex;
	/// @brief 原始统一资源定位符队列非满条件变量
	pthread_cond_t m_rawUrlNoFull;
	/// @brief 原始统一资源定位符队列非空条件变量
	pthread_cond_t m_rawUrlNoEmpty;

	/// @brief 解析统一资源定位符队列
	list<DnsUrl> m_dnsUrlQueue;
	/// @brief 解析统一资源定位符队列互斥锁
	mutable pthread_mutex_t m_dnsUrlMutex;
	/// @brief 解析统一资源定位符队列非满条件变量
	pthread_cond_t m_dnsUrlNoFull;
	/// @brief 解析统一资源定位符队列非空条件变量
	pthread_cond_t m_dnsUrlNoEmpty;
};

#endif // _URLQUEUES_H
