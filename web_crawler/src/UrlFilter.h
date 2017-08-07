/// @file UrlFilter.h
/// @brief 定义#UrlFilter接口类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _URLFILTER_H
#define _URLFILTER_H

/// @brief 统一资源定位符过滤器接口
class UrlFilter {
public:
	/// @brief 析构器
	virtual ~UrlFilter (void) {}

	/// @brief 判断某个统一资源定位符是否已经存在
	/// @retval true  存在
	/// @retval false 不存在
	/// @note 纯虚函数，子类根据不同过滤器的具体策略给出具体实现
	virtual bool exist (
		string const& strUrl ///< [in] 统一资源定位符
	)	= 0;
};

#endif // _URLFILTER_H
