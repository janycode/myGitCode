/// @file MaxDepth.cpp
/// @brief 实现#MaxDepth类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "MaxDepth.h"
#include "PluginMngr.h"

MaxDepth    g_pluginMaxDepth; // 最大深度插件对象
WebCrawler* g_app;            // 应用程序对象

// 插件初始化
// 成功返回true，失败返回false
// 根据最大深度插件的功能实现基类中的虚函数
bool MaxDepth::init (
	WebCrawler* app // [输入/输出] 应用程序对象
	) {
	// 以统一资源定位符插件的身份
	// 注册到应用程序对象的插件管理器中
	(g_app = app)->m_pluginMngr.registerUrlPlugin (this);

	// 返回成功
	return true;
}

// 插件处理
// 成功返回true，失败返回false
// 根据最大深度插件的功能实现基类中的虚函数
bool MaxDepth::handler (
	void* arg // [输入/输出] 插件参数
	) {
	// 原始统一资源定位符
	RawUrl* rawUrl = static_cast<RawUrl*> (arg);

	// 若配置器中的最大递归深度有效，且
	// 该统一资源定位符的链接深度已超限
	if (0 <= g_app->m_cfg.m_maxDepth &&
		g_app->m_cfg.m_maxDepth < rawUrl->m_depth) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"不抓过深(%d>%d)的统一资源定位符\"%s\"", rawUrl->m_depth,
			g_app->m_cfg.m_maxDepth, rawUrl->m_strUrl.c_str ());
		// 返回失败，不抓该统一资源定位符
		return false;
	}

	// 返回成功，抓取该统一资源定位符
	return true;
}
