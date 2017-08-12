/// @file UrlQueues.cpp
/// @brief 实现#UrlQueues类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "UrlFilter.h"

// 构造器
UrlQueues::UrlQueues (
	UrlFilter& filter // [输入] 统一资源定位符过滤器
	) : m_filter (filter) { // 初始化统一资源定位符过滤器
	// 初始化原始统一资源定位符队列互斥锁
	pthread_mutex_init (&m_rawUrlMutex, NULL);
	// 初始化原始统一资源定位符队列非满条件变量
	pthread_cond_init (&m_rawUrlNoFull, NULL);
	// 初始化原始统一资源定位符队列非空条件变量
	pthread_cond_init (&m_rawUrlNoEmpty, NULL);
	// 初始化解析统一资源定位符队列互斥锁
	pthread_mutex_init (&m_dnsUrlMutex, NULL);
	// 初始化解析统一资源定位符队列非满条件变量
	pthread_cond_init (&m_dnsUrlNoFull, NULL);
	// 初始化解析统一资源定位符队列非空条件变量
	pthread_cond_init (&m_dnsUrlNoEmpty, NULL);
}

// 析构器
UrlQueues::~UrlQueues (void) {
	// 销毁解析统一资源定位符队列非空条件变量
	pthread_cond_destroy (&m_dnsUrlNoEmpty);
	// 销毁解析统一资源定位符队列非满条件变量
	pthread_cond_destroy (&m_dnsUrlNoFull);
	// 销毁解析统一资源定位符队列互斥锁
	pthread_mutex_destroy (&m_dnsUrlMutex);
	// 销毁原始统一资源定位符队列非空条件变量
	pthread_cond_destroy (&m_rawUrlNoEmpty);
	// 销毁原始统一资源定位符队列非满条件变量
	pthread_cond_destroy (&m_rawUrlNoFull);
	// 销毁原始统一资源定位符队列互斥锁
	pthread_mutex_destroy (&m_rawUrlMutex);
}

// 压入原始统一资源定位符
void UrlQueues::pushRawUrl (
	RawUrl const& rawUrl // [输入] 原始统一资源定位符
	) {
	// 加锁原始统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_rawUrlMutex);

	// 若已被处理过
	if (m_filter.exist (rawUrl.m_strUrl))
		// 记录调试日志
		g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
			"不再处理已处理过的统一资源定位符\"%s\"",
			rawUrl.m_strUrl.c_str ());
	// 否则，调用统一资源定位符插件处理函数，若成功
	else if (g_app->m_pluginMngr.invokeUrlPlugins (
		const_cast<RawUrl*> (&rawUrl))) {
		// 若配置器中的原始统一资源定位符队列最大容量有效且到限
		while (0 <= g_app->m_cfg.m_maxRawUrls &&
			(size_t)g_app->m_cfg.m_maxRawUrls <= m_rawUrlQueue.size ())
			// 等待原始统一资源定位符队列非满条件变量
			pthread_cond_wait (&m_rawUrlNoFull, &m_rawUrlMutex);

		// 向原始统一资源定位符队列压入原始统一资源定位符
		m_rawUrlQueue.push_back (rawUrl);

		// 记录调试日志
		g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
			"原始统一资源定位符\"%s\"入队", rawUrl.m_strUrl.c_str ());

		// 若原始统一资源定位符队列由空变为非空
		if (m_rawUrlQueue.size () == 1)
			// 唤醒等待原始统一资源定位符队列非空条件变量的线程
			pthread_cond_signal (&m_rawUrlNoEmpty);
	}

	// 解锁原始统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_rawUrlMutex);
}

// 弹出原始统一资源定位符
// 返回原始统一资源定位符
RawUrl UrlQueues::popRawUrl (void) {
	// 加锁原始统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_rawUrlMutex);

	// 若原始统一资源定位符队列空
	while (m_rawUrlQueue.empty ())
		// 等待原始统一资源定位符队列非空条件变量
		pthread_cond_wait (&m_rawUrlNoEmpty, &m_rawUrlMutex);

	// 获取原始统一资源定位符队列首元素
	RawUrl rawUrl = m_rawUrlQueue.front ();
	// 从原始统一资源定位符队列弹出原始统一资源定位符
	m_rawUrlQueue.pop_front ();

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"原始统一资源定位符\"%s\"出队", rawUrl.m_strUrl.c_str ());

	// 若原始统一资源定位符队列由满变为非满
	if (m_rawUrlQueue.size () == (size_t)g_app->m_cfg.m_maxRawUrls - 1)
		// 唤醒等待原始统一资源定位符队列非满条件变量的线程
		pthread_cond_signal (&m_rawUrlNoFull);

	// 解锁原始统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_rawUrlMutex);
	// 返回原始统一资源定位符
	return rawUrl;
}

// 获取原始统一资源定位符数
// 返回原始统一资源定位符数
size_t UrlQueues::sizeRawUrl (void) const {
	// 加锁原始统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_rawUrlMutex);

	// 获取原始统一资源定位符队列大小
	size_t size = m_rawUrlQueue.size ();

	// 解锁原始统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_rawUrlMutex);
	// 返回原始统一资源定位符数
	return size;
}

/// 原始统一资源定位符队列空否
/// 空返回true，不空返回false
bool UrlQueues::emptyRawUrl (void) const {
	// 加锁原始统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_rawUrlMutex);

	// 获取原始统一资源定位符队列是否空
	bool empty = m_rawUrlQueue.empty ();

	// 解锁原始统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_rawUrlMutex);
	// 返回原始统一资源定位符队列是否空
	return empty;
}

// 原始统一资源定位符队列满否
// 满返回true，不满返回false
bool UrlQueues::fullRawUrl (void) const {
	// 加锁原始统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_rawUrlMutex);

	// 获取原始统一资源定位符队列是否满
	bool full = 0 <= g_app->m_cfg.m_maxRawUrls &&
		(size_t)g_app->m_cfg.m_maxRawUrls <= m_rawUrlQueue.size ();

	// 解锁原始统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_rawUrlMutex);
	// 返回原始统一资源定位符队列是否满
	return full;
}

// 清空原始统一资源定位符队列
void UrlQueues::clearRawUrl (void) {
	// 加锁原始统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_rawUrlMutex);

	// 清空原始统一资源定位符队列
	m_rawUrlQueue.clear ();
	// 唤醒等待原始统一资源定位符队列非满条件变量的线程
	pthread_cond_signal (&m_rawUrlNoFull);

	// 解锁原始统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_rawUrlMutex);
}

// 压入解析统一资源定位符
void UrlQueues::pushDnsUrl (
	DnsUrl const& dnsUrl // [输入] 解析统一资源定位符
	) {
	// 加锁解析统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_dnsUrlMutex);

	// 若配置器中的解析统一资源定位符队列最大容量有效且到限
	while (0 <= g_app->m_cfg.m_maxDnsUrls &&
		(size_t)g_app->m_cfg.m_maxDnsUrls <= m_dnsUrlQueue.size ())
		// 等待解析统一资源定位符队列非满条件变量
		pthread_cond_wait (&m_dnsUrlNoFull, &m_dnsUrlMutex);

	// 向解析统一资源定位符队列压入解析统一资源定位符
	m_dnsUrlQueue.push_back (dnsUrl);

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"解析统一资源定位符\"ip=%s, port=%d, path=%s\"入队",
		dnsUrl.m_ip.c_str (), dnsUrl.m_port, dnsUrl.m_path.c_str ());

	// 若解析统一资源定位符队列由空变为非空
	if (m_dnsUrlQueue.size () == 1)
		// 唤醒等待解析统一资源定位符队列非空条件变量的线程
		pthread_cond_signal (&m_dnsUrlNoEmpty);

	// 解锁解析统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_dnsUrlMutex);
}

// 弹出解析统一资源定位符
// 返回解析统一资源定位符
DnsUrl UrlQueues::popDnsUrl (void) {
	// 加锁解析统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_dnsUrlMutex);

	// 若解析统一资源定位符队列空
	while (m_dnsUrlQueue.empty ())
		// 等待解析统一资源定位符队列非空条件变量
		pthread_cond_wait (&m_dnsUrlNoEmpty, &m_dnsUrlMutex);

	// 获取解析统一资源定位符队列首元素
	DnsUrl dnsUrl = m_dnsUrlQueue.front ();
	// 从解析统一资源定位符队列弹出解析统一资源定位符
	m_dnsUrlQueue.pop_front ();

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"解析统一资源定位符\"ip=%s, port=%d, path=%s\"出队",
		dnsUrl.m_ip.c_str (), dnsUrl.m_port, dnsUrl.m_path.c_str ());

	// 若解析统一资源定位符队列由满变为非满
	if (m_dnsUrlQueue.size () == (size_t)g_app->m_cfg.m_maxDnsUrls - 1)
		// 唤醒等待解析统一资源定位符队列非满条件变量的线程
		pthread_cond_signal (&m_dnsUrlNoFull);

	// 解锁解析统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_dnsUrlMutex);
	// 返回解析统一资源定位符
	return dnsUrl;
}

// 获取解析统一资源定位符数
// 返回解析统一资源定位符数
size_t UrlQueues::sizeDnsUrl (void) const {
	// 加锁解析统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_dnsUrlMutex);

	// 获取解析统一资源定位符队列大小
	size_t size = m_dnsUrlQueue.size ();

	// 解锁解析统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_dnsUrlMutex);
	// 返回解析统一资源定位符数
	return size;
}

// 解析统一资源定位符队列空否
// 空返回true，不空返回false
bool UrlQueues::emptyDnsUrl (void) const {
	// 加锁解析统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_dnsUrlMutex);

	// 获取解析统一资源定位符队列是否空
	bool empty = m_dnsUrlQueue.empty ();

	// 解锁解析统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_dnsUrlMutex);
	// 返回解析统一资源定位符队列是否空
	return empty;
}

// 解析统一资源定位符队列满否
// 满返回true，不满返回false
bool UrlQueues::fullDnsUrl (void) const {
	// 加锁解析统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_dnsUrlMutex);

	// 获取解析统一资源定位符队列是否满
	bool full = 0 <= g_app->m_cfg.m_maxDnsUrls &&
		(size_t)g_app->m_cfg.m_maxDnsUrls <= m_dnsUrlQueue.size ();

	// 解锁解析统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_dnsUrlMutex);
	// 返回解析统一资源定位符队列是否满
	return full;
}

// 清空解析统一资源定位符队列
void UrlQueues::clearDnsUrl (void) {
	// 加锁解析统一资源定位符队列互斥锁
	pthread_mutex_lock (&m_dnsUrlMutex);

	// 清空解析统一资源定位符队列
	m_dnsUrlQueue.clear ();
	// 唤醒等待解析统一资源定位符队列非满条件变量的线程
	pthread_cond_signal (&m_dnsUrlNoFull);

	// 解锁解析统一资源定位符队列互斥锁
	pthread_mutex_unlock (&m_dnsUrlMutex);
}

// 从超文本标记语言页面内容中抽取统一资源定位符
void UrlQueues::extractUrl (
	char const*   html,  // [输入] 超文本标记语言页面内容字符串
	DnsUrl const& dnsUrl // [输入] 被抽取页面解析统一资源定位符
	) {
	// 正则表达式
	regex_t ex;

	// 编译正则表达式：href="\s*\([^ >"]*\)\s*"
	//     \s - 匹配任意空白字符(空格、制表、换页等)
	//      * - 重复前一个匹配项任意次
	//     \( - 子表达式左边界
	//     \) - 子表达式右边界
	// [^ >"] - 匹配任意不是空格、大于号和双引号的字符
	int error = regcomp (&ex, "href=\"\\s*\\([^ >\"]*\\)\\s*\"", 0);
	// 若失败
	if (error) {
		// 错误信息缓冲区
		char errInfo[1024];
		// 获取正则表达式编译错误信息
		regerror (error, &ex, errInfo, sizeof (errInfo) /
			sizeof (errInfo[0]));
		// 记录一般错误日志
		g_app->m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
			"regcomp: %s", errInfo);
	}

	// 匹配集合
	regmatch_t match[2];

	// 在超文本标记语言页面内容字符串中，
	// 查找所有与正则表达式匹配的内容
	while (regexec (&ex, html, sizeof (match) /
		sizeof (match[0]), match, 0) != REG_NOMATCH) {
		// regex : href="\s*\([^ >"]*\)\s*"
		// html  : ...href="  /software/download.html  "...
		//            |       |<-----match[1]------>|  |
		//            |     rm_so                 rm_eo|
		//            |<-------------match[0]--------->|
		//          rm_so                            rm_eo

		// 匹配子表达式的内容首地址
		html += match[1].rm_so;
		// 匹配子表达式的内容字符数
		size_t len = match[1].rm_eo - match[1].rm_so;
		// 匹配子表达式的内容字符串，即超链接中的统一资源定位符
		string strUrl (html, len);
		// 移至匹配主表达式的内容后，以备在下一轮循环中继续查找
		html += len + match[0].rm_eo - match[1].rm_eo;

		// 若是二进制资源
		if (isBinary (strUrl))
			// 继续下一轮循环
			continue;

		// 若添加域名失败
		if (! dnsUrl.attachDomain (strUrl))
			// 继续下一轮循环
			continue;

		// 记录调试日志
		g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
			"抽取到一个深度为%d的统一资源定位符\"%s\"",
			dnsUrl.m_depth + 1, strUrl.c_str ());

		// 若规格化失败
		if (! RawUrl::normalized (strUrl)) {
			// 记录警告日志
			g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
				"规格化统一资源定位符\"%s\"失败", strUrl.c_str ());
			// 继续下一轮循环
			continue;
		}

		// 压入原始统一资源定位符队列
		pushRawUrl (RawUrl (strUrl, RawUrl::ETYPE_HTML,
			dnsUrl.m_depth + 1));
	}

	// 释放正则表达式
	regfree (&ex);
}

// 判断某统一资源定位符所表示的资源是否是二进制资源
// 是二进制资源返回true，非二进制资源返回false
bool UrlQueues::isBinary (
	string const& strUrl // [输入] 统一资源定位符字符串
	) {
	// 统一资源定位符字符串中'.'字符的下标
	string::size_type pos;
	// 若统一资源定位符字符串中最后一个'.'字符，连同其后的子串
	// 构成二进制资源的文件扩展名，则返回true，否则返回false
	return (pos = strUrl.find_last_of ('.')) != string::npos &&
		string (".jpg.jpeg.gif.png.ico.bmp.swf").find (
			strUrl.substr (pos)) != string::npos;
}
/*
// 将以毫秒表示的相对时间换算为以秒和纳秒表示的绝对时间
// 返回以秒和纳秒表示的绝对时间
timespec UrlQueues::getTimespec (
	long msec // [输入] 以毫秒表示的相对时间
	) {
	// 以秒和微秒表示的时间
    timeval tv;
    // 获取当前系统时间
    gettimeofday (&tv, NULL);

	// 用以秒和微秒表示的当前系统时间，加上以毫秒表示
	// 的相对时间，得到以秒和纳秒表示的未来绝对时间
	timespec ts = {tv.tv_sec, (tv.tv_usec + msec * 1000) * 1000};
	return ts;
}
*/
