/// @file Http.h
/// @brief 定义#HttpHeader类和#HttpResponse类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _HTTP_H
#define _HTTP_H

#include "Url.h"

/// @brief 超文本传输协议响应包头
class HttpHeader {
public:
	/// @brief 状态码
	int m_statusCode;
	/// @brief 内容类型
	string m_contentType;
};

/// @brief 超文本传输协议响应
class HttpResponse {
public:
	/// @brief 构造器
	HttpResponse (
		DnsUrl const& dnsUrl ///< [in] 服务器统一资源定位符
	) : m_dnsUrl (dnsUrl), m_body (NULL), m_len (0) {}

	/// @brief 析构器
	~HttpResponse (void) {
		if (m_body) {
			free (m_body);
			m_body = NULL;
		}
		m_len = 0;
	}

	/// @brief 服务器统一资源定位符
	DnsUrl m_dnsUrl;
	/// @brief 超文本传输协议响应包头
	HttpHeader m_header;
	/// @brief 超文本传输协议响应包体指针
	char* m_body;
	/// @brief 超文本传输协议响应包体长度
	size_t m_len;
};

#endif // _HTTP_H
