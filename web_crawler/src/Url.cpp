/// @file Url.cpp
/// @brief 实现#RawUrl类和#DnsUrl类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "Url.h"
#include "StrKit.h"

// 构造器
RawUrl::RawUrl (
	string const& strUrl,                   // [输入] 统一资源定位符字符串
	ETYPE         type  /* = ETYPE_HTML */, // [输入] 资源类型
	int           depth /* = 0 */           // [输入] 链接深度
	) : m_strUrl (strUrl),  // 初始化统一资源定位符字符串
	    m_type   (type),    // 初始化资源类型
	    m_depth  (depth) {} // 初始化链接深度

// 规格化
// 成功返回true，失败返回false
// 删除协议标签(http://或https://)和结尾分隔符(/)
bool RawUrl::normalized (
	string& strUrl // [输入/输出] 待规格化统一资源定位符字符串
	) {
	// 将待规格化统一资源定位符字符串复制到临时变量
	string strTmp = strUrl;
	// 修剪临时变量中的待规格化统一资源定位符字符串
	StrKit::trim (strTmp);

	// 若待规格化统一资源定位符字符串为空串
	if (strTmp.empty ())
		// 返回失败
		return false;

	// 若待规格化统一资源定位符字符串以"http://"开头
	if (! strTmp.find ("http://", 0))
		// 剪除该协议子串
		strTmp = strTmp.substr (7);
	// 否则
	else
	// 若待规格化统一资源定位符字符串以"https://"开头
	if (! strTmp.find ("https://", 0))
		// 剪除该协议子串
		strTmp = strTmp.substr (8);

	// 若待规格化统一资源定位符字符串以'/'结尾
	if (*(strTmp.end () - 1) == '/')
		// 剪除该路径分隔符
		strTmp.erase (strTmp.end () - 1);

	// 若待规格化统一资源定位符字符串过长(大于128个字符)
	if (strTmp.size () > 128)
		// 返回失败
		return false;

	// 将临时变量中已规格化的统一资源定位符字符串复制到参数中
	strUrl = strTmp;
	// 返回成功
	return true;
}

////////////////////////////////////////////////////////////////////////

// 构造器
DnsUrl::DnsUrl (
	RawUrl const& rawUrl // [输入] 原始统一资源定位符
	) : RawUrl (rawUrl) { // 初始化原始统一资源定位符基类子对象
	// 在从基类继承的统一资源定位符字符串中查找第一个'/'
	string::size_type pos = m_strUrl.find_first_of ('/');
	// 若没找到
	if (pos == string::npos)
		// 整个统一资源定位符字符串都认作服务器域名
		m_domain = m_strUrl;
	// 否则
	else {
		// 统一资源定位符字符串中第一个'/'之前为服务器域名
		m_domain = m_strUrl.substr (0, pos);
		// 之后为资源路径。注意作为服务器域名和
		// 资源路径之间分隔符的'/'不放在路径中
		m_path = m_strUrl.substr (pos + 1);
	}

	// 在服务器域名中查找最后一个':'。若找到了，则将其后
	// 的子串转换为整数作为服务器通信端口；若没有找到或转
	// 换后的端口号非法，则将服务器通信端口设置为缺省值80
	if ((pos = m_domain.find_last_of (':')) == string::npos ||
		! (m_port = atoi (m_domain.substr (pos + 1).c_str ())))
		m_port = 80;
}

// 转换为文件名字符串
// 返回文件名字符串
string DnsUrl::toFilename (void) const {
	// 用服务器域名初始化文件名字符串
	string filename = m_domain;

	// 若资源路径非空
	if (! m_path.empty ())
		// 在文件名字符串中追加"/"和资源路径
		(filename += "/") += m_path;

	// 逐个查找文件名字符串中的每个'/'
	for (string::size_type pos = 0; (pos = filename.find (
		'/', pos)) != string::npos; ++pos)
		// 若找到，则将其替换为"_"
		filename.replace (pos, 1, "_");

	// 若资源类型为超文本标记语言
	if (m_type == ETYPE_HTML) {
		// 在文件名字符串中查找最后一个'.'
		string::size_type pos = filename.find_last_of ('.');
		// 若没找到，或虽然找到但连同其后
		// 的子串既不是".htm"也不是".html"
		if (pos == string::npos || (
			filename.substr (pos) != ".htm" &&
			filename.substr (pos) != ".html"))
			// 在文件名字符串中追加扩展名".html"
			filename += ".html";
	}

	// 返回文件名字符串
	return filename;
}

// 添加域名
// 成功返回true，失败返回false
bool DnsUrl::attachDomain (
	string& strUrl // [输入/输出] 待加域名统一资源定位符字符串
	) const {
	// 若待加域名统一资源定位符字符串以"http"开头，说明已有域名
	if (! strUrl.find ("http", 0))
		// 直接返回成功
		return true;

	// 若待加域名统一资源定位符字符串为空，或首字符不是'/'
	if (strUrl.empty () || strUrl[0] != '/')
		// 返回失败
		return false;

	// 将服务器域名插到待加域名统一资源定位符字符串首字符之前
	strUrl.insert (0, m_domain);
	// 返回成功
	return true;
}
