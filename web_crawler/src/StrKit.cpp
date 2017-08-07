/// @file StrKit.cpp
/// @brief 实现#StrKit类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "StrKit.h"

// 字符串拼接
// 返回拼接后的字符串
string StrKit::strcat (
	char const* str1, // [输入] 字符串1
	char const* str2, // [输入] 字符串2
	...               // [输入] 字符串n
	) {
	// 用第一个参数字符串初始化结果字符串
	string str = str1;
	// 将第二个参数字符串拼接到结果字符串末尾
	str += str2;

	// 变长参数表
	va_list ap;
	// 用str2以后的参数初始化变长参数表
	va_start (ap, str2);

	// 指向变长参数表中参数的指针
	char const* strx = NULL;
	// 依次获取变长参数表中的每个字符串参数
	while (strx = va_arg (ap, char const*))
		// 拼接到结果字符串末尾
		str += strx;

	// 销毁变长参数表
	va_end (ap);
	// 返回结果字符串
	return str;
}

// 字符串修剪
// 返回被修剪过的参数字符串本身
// 截去字符串的首尾空白字符(空格、制表、回车、换行等)
string& StrKit::trim (
	string& str // [输入/输出] 待修剪字符串
	) {
	// 在待修剪字符串查找第一个和最后一个非空白字符
	string::size_type first = str.find_first_not_of (" \t\r\n"),
		last = str.find_last_not_of (" \t\r\n");

	// 若没有找到任何非空白字符，说明待修剪字
	// 符串要么是空串，要么全部由空白字符组成
	if (first == string::npos || last == string::npos)
		// 直接清空
		str.clear ();
	// 否则
	else
		// 截取从第一个到最后一个非空白字符之间的子串
		str = str.substr (first, last - first + 1);

	// 返回被修剪过的参数字符串本身
	return str;
}

// 字符串拆分
// 返回被拆分出的子串向量
// 以delim中的字符作为分隔符，对str字符串进行拆分，
// 并对每个被拆分出的子串做修剪，拆分次数不超过limit，
// 除非该参数的值为0
vector<string> StrKit::split (
	string const& str,            // [输入] 待拆分字符串
	string const& delim,          // [输入] 分隔符字符串
	int           limit /* = 0 */ // [输入] 拆分次数限制
	) {
	// 存放拆分结果的子串向量
	vector<string> strv;

	// 待拆分字符串临时缓冲区，注意多分配一个字符放'\0'
	char temp[str.size () + 1];
	// 将待拆分字符串，连同终止空字符('\0')一起，复制到临时缓冲区中
	strcpy (temp, str.c_str ());
	// strtok要求待拆分字符串所在内存必须可写

	//   delim : " ,.:;()$"
	//   limit : 3
	//    temp : The quick,brown.fox:jumps;over(the)lazy$dog/0
	//           t  /t    /t    /t  /t    /t   /t  /t   /t
	//           o  0o    0o    0o  0o    0o   0o  0o   0o
	//           k   k     k     k   k     k    k   k    k
	//           e   e     e     e   e     e    e   e    e
	//           n   n     n     n   n     n    n   n    n
	//    strv : The
	//               quick
	//                     brown
	//                           fox:jumps;over(the)lazy$dog
	// --limit : 2   1     0

	// 依次提取待拆分字符串中每个被分隔符字符串中的字符分隔的子串
	for (char* token = strtok (temp, delim.c_str ());
		token; token = strtok (NULL, delim.c_str ())) {
		// 被拆分出的子串
		string part = token;
		// 经修剪后压入存放拆分结果的子串向量
		strv.push_back (trim (part));

		// 若拆分次数限制已到(若limit参数取缺省值0，则此条件永远不
		// 可能满足，即不限制拆分次数)，且本次所拆并非最后一个子串
		if (! --limit && (token += strlen (token)) - temp <
			(int)str.size ()) {
			// 将待拆分字符串的其余部分一次
			// 性压入存放拆分结果的子串向量
			strv.push_back (trim (part = ++token));
			// 提前结束拆分循环
			break;
		}
	}

	// 返回存放拆分结果的子串向量
	return strv;
}
/*
// 测试用例
int main (void) {
	string str = StrKit::strcat (
		"  123  = ", " 456 ", " =  789  ", NULL);
	cout << '[' << str << ']' << endl;

	cout << '[' << StrKit::trim (str) << ']' << endl;

	vector<string> strv = StrKit::split (str, "=", 1);
	for (vector<string>::const_iterator it = strv.begin ();
		it != strv.end (); ++it)
		cout << '[' << *it << ']' << endl;
	return 0;
}
*/
