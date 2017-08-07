/// @file Log.h
/// @brief 声明#Log类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _LOG_H
#define _LOG_H

/// @brief 日志
class Log {
public:
	/// @brief 日志等级 - 调试
	static int const LEVEL_DBG = 0;
	/// @brief 日志等级 - 信息
	static int const LEVEL_INF = 1;
	/// @brief 日志等级 - 警告
	static int const LEVEL_WAR = 2;
	/// @brief 日志等级 - 一般错误
	static int const LEVEL_ERR = 3;
	/// @brief 日志等级 - 致命错误
	static int const LEVEL_CRT = 4;

	/// @brief 按格式打印日志
	void printf (
		int         level,  ///< [in] 日志等级
		char const* file,   ///< [in] 源码文件
		int         line,   ///< [in] 源码行号
		char const* format, ///< [in] 格式化串
		...                 ///< [in] 打印内容，数量和类型与格式化串format中的格式化标记对应匹配
	)	const;
	/*
	/// @brief 按十六进制打印内存
	void printh (
		void*  buf, ///< [in] 内存缓冲区
		size_t len  ///< [in] 内存字节数
	)	const;
	*/
private:
	/// @brief 日志等级标签数组 \brief
	/// 从低到高依次为：
	/// - dbg - 调试
	/// - inf - 信息
	/// - war - 警告
	/// - err - 一般错误
	/// - crt - 致命错误
	static char const* s_levels[];
};

#endif // _LOG_H
