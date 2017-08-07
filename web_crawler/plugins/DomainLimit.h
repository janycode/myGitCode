/// @file DomainLimit.h
/// @brief 声明#DomainLimit类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _DOMAINLIMIT_H
#define _DOMAINLIMIT_H

#include "Plugin.h"

/// @brief 域名限制插件
class DomainLimit : public Plugin {
private:
	/// @brief 插件初始化
	/// @retval true  成功
	/// @retval false 失败
	/// @note 根据域名限制插件的功能实现基类中的虚函数
	bool init (
		WebCrawler* app ///< [in,out] 应用程序对象
	);
	/// @brief 插件处理
	/// @retval true  成功
	/// @retval false 失败
	/// @note 根据域名限制插件的功能实现基类中的虚函数
	bool handler (
		void* arg ///< [in,out] 插件参数
	);

	/// @brief 包含前缀字符串向量
	vector<string> m_includePrefixes;
	/// @brief 排除前缀字符串向量
	vector<string> m_excludePrefixes;
};

#endif // _DOMAINLIMIT_H
