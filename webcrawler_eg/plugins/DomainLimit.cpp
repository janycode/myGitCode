/// @file DomainLimit.cpp
/// @brief 实现#DomainLimit类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "DomainLimit.h"
#include "PluginMngr.h"
#include "StrKit.h"

DomainLimit g_pluginDomainLimit; // 域名限制插件对象
WebCrawler* g_app;               // 应用程序对象

// 插件初始化
// 成功返回true，失败返回false
// 根据域名限制插件的功能实现基类中的虚函数
bool DomainLimit::init (
	WebCrawler* app // [输入/输出] 应用程序对象
	) {
	// 以统一资源定位符插件的身份
	// 注册到应用程序对象的插件管理器中
	(g_app = app)->m_pluginMngr.registerUrlPlugin (this);

	// 拆分配置器中的包含前缀字符串，以逗号为分隔符，不限拆分次数
	m_includePrefixes = StrKit::split (
		g_app->m_cfg.m_includePrefixes, ",", 0);
	// 拆分配置器中的排除前缀字符串，以逗号为分隔符，不限拆分次数
	m_excludePrefixes = StrKit::split (
		g_app->m_cfg.m_excludePrefixes, ",", 0);;

	// 返回成功
	return true;
}

// 插件处理
// 成功返回true，失败返回false
// 根据域名限制插件的功能实现基类中的虚函数
bool DomainLimit::handler (
	void* arg // [输入/输出] 插件参数
	) {
	// 原始统一资源定位符
	RawUrl* rawUrl = static_cast<RawUrl*> (arg);

	// 若是种子链接(链接深度为0)或非超文本标记语言
	if (! rawUrl->m_depth || rawUrl->m_type != RawUrl::ETYPE_HTML)
		// 返回成功，抓取该统一资源定位符
		return true;

	// 字符串向量只读迭代器
	vector<string>::const_iterator it;

	// 统一资源定位符字符串以哪个包含前缀开头
	for (it = m_includePrefixes.begin ();
		it != m_includePrefixes.end () &&
		rawUrl->m_strUrl.find (*it, 0); ++it);

	// 若没有与统一资源定位符字符串匹配的包含前缀
	if (! m_includePrefixes.empty () &&
		it == m_includePrefixes.end ()) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"不抓不在包含集\"%s\"中的统一资源定位符\"%s\"",
			g_app->m_cfg.m_includePrefixes.c_str (),
			rawUrl->m_strUrl.c_str ());
		// 返回失败，不抓该统一资源定位符
		return false;
	}

	// 统一资源定位符字符串以哪个排除前缀开头
	for (it = m_excludePrefixes.begin ();
		it != m_excludePrefixes.end () &&
		rawUrl->m_strUrl.find (*it, 0); ++it);

	// 若存在与统一资源定位符字符串匹配的排除前缀
	if (it != m_excludePrefixes.end ()) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"不抓排除集\"%s\"中的统一资源定位符\"%s\"",
			g_app->m_cfg.m_excludePrefixes.c_str (),
			rawUrl->m_strUrl.c_str ());
		// 返回失败，不抓该统一资源定位符
		return false;
	}

	// 返回成功，抓取该统一资源定位符
	return true;
}
