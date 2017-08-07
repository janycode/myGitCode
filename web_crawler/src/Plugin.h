/// @file Plugin.h
/// @brief 定义#Plugin接口类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _PLUGIN_H
#define _PLUGIN_H

class WebCrawler;

/// @brief 插件接口
class Plugin {
public:
	/// @brief 析构器
	virtual ~Plugin (void) {}

	/// @brief 插件初始化
	/// @retval true  成功
	/// @retval false 失败
	/// @note 纯虚函数，子类根据不同的插件功能给出具体实现
	virtual bool init (
		WebCrawler* app ///< [in,out] 应用程序对象
	)	= 0;
	/// @brief 插件处理
	/// @retval true  成功
	/// @retval false 失败
	/// @note 纯虚函数，子类根据不同插件的具体功能给出具体实现
	virtual bool handler (
		void* arg ///< [in,out] 插件参数
	)	= 0;
};

#endif // _PLUGIN_H
