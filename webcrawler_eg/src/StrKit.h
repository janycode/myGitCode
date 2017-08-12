/// @file StrKit.h
/// @brief 声明#StrKit类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _STRKIT_H
#define _STRKIT_H

/// @brief 字符串工具包
class StrKit {
public:
	/// @brief 字符串拼接
	/// @return 拼接后的字符串
	static string strcat (
		char const* str1, ///< [in] 字符串1
		char const* str2, ///< [in] 字符串2
		...               ///< [in] 字符串n
	);
	/// @brief 字符串修剪
	/// @return 被修剪过的参数字符串本身
	/// @remark 截去字符串的首尾空白字符(空格、制表、回车、换行等)
	static string& trim (
		string& str ///< [in,out] 待修剪字符串
	);
	/// @brief 字符串拆分
	/// @return 被拆分出的子串向量
	/// @remark 以delim中的字符作为分隔符，对str字符串进行拆分，并对每个被拆分出的子串做修剪，拆分次数不超过limit，除非该参数的值为0
	static vector<string> split (
		string const& str,      ///< [in] 待拆分字符串
		string const& delim,    ///< [in] 分隔符字符串
		int           limit = 0 ///< [in] 拆分次数限制
	);
};

#endif // _STRKIT_H
