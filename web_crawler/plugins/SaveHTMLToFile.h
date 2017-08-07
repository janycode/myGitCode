/// @file SaveHTMLToFile.h
/// @brief 声明#SaveHTMLToFile类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _SAVEHTMLTOFILE_H
#define _SAVEHTMLTOFILE_H

#include "Plugin.h"

/// @brief 超文本标记语言文件存储插件
class SaveHTMLToFile : public Plugin {
private:
	/// @brief 插件初始化
	/// @retval true  成功
	/// @retval false 失败
	/// @note 根据超文本标记语言文件存储插件的功能实现基类中的虚函数
	bool init (
		WebCrawler* app ///< [in,out] 应用程序对象
	);
	/// @brief 插件处理
	/// @retval true  成功
	/// @retval false 失败
	/// @note 根据超文本标记语言文件存储插件的功能实现基类中的虚函数
	bool handler (
		void* arg ///< [in,out] 插件参数
	);
};

#endif // _SAVEHTMLTOFILE_H
