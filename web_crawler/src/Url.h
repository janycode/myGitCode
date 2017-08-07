/// @file Url.h
/// @brief 声明#RawUrl类和#DnsUrl类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _URL_H
#define _URL_H

/// @brief 原始统一资源定位符
class RawUrl {
public:
	/// @brief 资源类型
	typedef enum tag_Type {
		ETYPE_HTML, ///< 超文本标记语言
		ETYPE_IMAGE ///< 图像
	}	ETYPE;

	/// @brief 构造器
	RawUrl (
		string const& strUrl,             ///< [in] 统一资源定位符字符串
		ETYPE         type  = ETYPE_HTML, ///< [in] 资源类型
		int           depth = 0           ///< [in] 链接深度
	);

	/// @brief 规格化
	/// @retval true  成功
	/// @retval false 失败
	/// @remark 删除协议标签(http://或https://)和结尾分隔符(/)
	static bool normalized (
		string& strUrl ///< [in,out] 待规格化统一资源定位符字符串
	);

	/// @brief 统一资源定位符字符串
	string m_strUrl;
	/// @brief 资源类型
	ETYPE m_type;
	/// @brief 链接深度
	int m_depth;
};

/// @brief 解析统一资源定位符
class DnsUrl : public RawUrl {
public:
	/// @brief 构造器
	explicit DnsUrl (
		RawUrl const& rawUrl ///< [in] 原始统一资源定位符
	);

	/// @brief 转换为文件名字符串
	/// @return 文件名字符串
	string toFilename (void) const;
	/// @brief 添加域名
	/// @retval true  成功
	/// @retval false 失败
	bool attachDomain (
		string& strUrl ///< [in,out] 待加域名统一资源定位符字符串
	)	const;

	/// @brief 服务器域名
	string m_domain;
	/// @brief 资源路径
	string m_path;
	/// @brief 服务器IP地址
	string m_ip;
	/// @brief 服务器通信端口
	short m_port;
};

#endif // _URL_H
