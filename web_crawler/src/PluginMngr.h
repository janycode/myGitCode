/// @file PluginMngr.h
/// @brief 声明#PluginMngr类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _PLUGINMNGR_H
#define _PLUGINMNGR_H

class Plugin;

/// @brief 插件管理器
class PluginMngr {
public:
	/// @brief 加载插件
	void load (void);

	/// @brief 注册统一资源定位符插件
	/// @remark 统一资源定位符插件通过此接口将其自身注册到插件管理器中
	void registerUrlPlugin (
		Plugin* plugin ///< [in] 统一资源定位符插件
	);
	/// @brief 注册超文本传输协议响应包头插件
	/// @remark 超文本传输协议响应包头插件通过此接口将其自身注册到插件管理器中
	void registerHeaderPlugin (
		Plugin* plugin ///< [in] 超文本传输协议响应包头插件
	);
	/// @brief 注册超文本标记语言插件
	/// @remark 超文本标记语言插件通过此接口将其自身注册到插件管理器中
	void registerHtmlPlugin (
		Plugin* plugin ///< [in] 超文本标记语言插件
	);

	/// @brief 调用统一资源定位符插件处理函数
	/// @retval true  成功
	/// @retval false 失败
	/// @remark 依次调用每个统一资源定位符插件处理函数，只要有一个失败即返回失败
	bool invokeUrlPlugins (
		void* arg ///< [in,out] 插件参数
	)	const;
	/// @brief 调用超文本传输协议响应包头插件处理函数
	/// @retval true  成功
	/// @retval false 失败
	/// @remark 依次调用每个超文本传输协议响应包头插件处理函数，只要有一个失败即返回失败
	bool invokeHeaderPlugins (
		void* arg ///< [in,out] 插件参数
	)	const;
	/// @brief 调用超文本标记语言插件处理函数
	/// @remark 依次调用每个超文本标记语言插件处理函数，忽略其成功失败
	void invokeHtmlPlugins (
		void* arg ///< [in,out] 插件参数
	)	const;

private:
	/// @brief 统一资源定位符插件向量
	vector<Plugin*> m_urlPlugins;
	/// @brief 超文本传输协议响应包头插件向量
	vector<Plugin*> m_headerPlugins;
	/// @brief 超文本标记语言插件向量
	vector<Plugin*> m_htmlPlugins;
};

#endif // _PLUGINMNGR_H
