/// @file PluginMngr.cpp
/// @brief 实现#PluginMngr类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "Plugin.h"
#include "StrKit.h"

// 加载插件
void PluginMngr::load (void) {
	// 遍历配置器中的插件列表向量
	for (vector<string>::const_iterator it =
		g_app->m_cfg.m_loadPlugins.begin (); it !=
		g_app->m_cfg.m_loadPlugins.end (); ++it) {
		// 利用配置器中的插件路径，构造插件共享库路径：
		// <插件路径>/<插件名>.so，例如：
		// ../plugins/MaxDepth.so
		// ../plugins/DomainLimit.so，等等
		string path = StrKit::strcat (
			g_app->m_cfg.m_pluginsPath.c_str (), "/",
			it->c_str (), ".so", NULL);

		// 记录调试日志
		g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
			"加载\"%s\"插件", path.c_str ());

		// 打开插件共享库
		void* handle = dlopen (path.c_str (), RTLD_GLOBAL | RTLD_NOW);
		// 若失败
		if (! handle)
			// 记录一般错误日志
			g_app->m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
				"dlopen: %s", dlerror ());

		// 利用插件名，构造插件对象名：
		// g_plugin<插件名>，例如：
		// g_pluginMaxDepth
		// g_pluginDomainLimit，等等
		string symbol = "g_plugin";
		symbol += *it;

		// 从插件共享库中获取插件对象地址，并转换为其基类类型的指针
		Plugin* plugin = (Plugin*)dlsym (handle, symbol.c_str ());
		// 若失败
		if (! plugin)
			// 记录一般错误日志
			g_app->m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
				"dlsym: %s", dlerror ());

		// 通过插件基类调用插件子类的初始化接口
		plugin->init (g_app);
	}
}

// 注册统一资源定位符插件
// 统一资源定位符插件通过此接口将其自身注册到插件管理器中
void PluginMngr::registerUrlPlugin (
	Plugin* plugin // [输入] 统一资源定位符插件
	) {
	// 将统一资源定位符插件指针压入统一资源定位符插件向量
	m_urlPlugins.push_back (plugin);
}

// 注册超文本传输协议响应包头插件
// 超文本传输协议响应包头插件通过此接口将其自身注册到插件管理器中
void PluginMngr::registerHeaderPlugin (
	Plugin* plugin // [输入] 超文本传输协议响应包头插件
	) {
	// 将超文本传输协议响应包头插件指针压
	// 入超文本传输协议响应包头插件向量
	m_headerPlugins.push_back (plugin);
}

// 注册超文本标记语言插件
// 超文本标记语言插件通过此接口将其自身注册到插件管理器中
void PluginMngr::registerHtmlPlugin (
	Plugin* plugin // [输入] 超文本标记语言插件
	) {
	// 将超文本标记语言插件指针压入超文本标记语言插件向量
	m_htmlPlugins.push_back (plugin);
}

// 调用统一资源定位符插件处理函数
// 成功返回true，失败返回false
// 依次调用每个统一资源定位符插件处理函数，只要有一个失败即返回失败
bool PluginMngr::invokeUrlPlugins (
	void* arg // [输入/输出] 插件参数
	) const {
	// 遍历统一资源定位符插件向量
	for (vector<Plugin*>::const_iterator it = m_urlPlugins.begin ();
		it != m_urlPlugins.end (); ++it)
		// 依次调用每个统一资源定位符插件的插件处理接口，若失败
		if (! (*it)->handler (arg))
			// 返回失败
			return false;

	// 返回成功
	return true;
}

// 调用超文本传输协议响应包头插件处理函数
// 成功返回true，失败返回false
// 依次调用每个超文本传输协议响应包头插
// 件处理函数，只要有一个失败即返回失败
bool PluginMngr::invokeHeaderPlugins (
	void* arg // [输入/输出] 插件参数
	) const {
	// 遍历超文本传输协议响应包头插件向量
	for (vector<Plugin*>::const_iterator it = m_headerPlugins.begin ();
		it != m_headerPlugins.end (); ++it)
		// 依次调用每个超文本传输协议响应
		// 包头插件的插件处理接口，若失败
		if (! (*it)->handler (arg))
			// 返回失败
			return false;

	// 返回成功
	return true;
}

// 调用超文本标记语言插件处理函数
// 依次调用每个超文本标记语言插件处理函数，忽略其成功失败
void PluginMngr::invokeHtmlPlugins (
	void* arg // [输入/输出] 插件参数
	) const {
	// 遍历超文本标记语言插件向量
	for (vector<Plugin*>::const_iterator it = m_htmlPlugins.begin ();
		it != m_htmlPlugins.end (); ++it)
		// 依次调用每个超文本标记语言插件
		// 的插件处理接口，不管成功失败
		(*it)->handler (arg);
}
