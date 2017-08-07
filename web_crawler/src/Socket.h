/// @file Socket.h
/// @brief 声明#Socket类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _SOCKET_H
#define _SOCKET_H

#include "Url.h"
#include "Http.h"

/// @brief 套接字
class Socket {
public:
	/// @brief 构造器
	Socket (
		DnsUrl const& dnsUrl ///< [in] 服务器统一资源定位符
	);
	/// @brief 析构器
	~Socket (void);

	/// @brief 发送超文本传输协议请求
	/// @retval true  成功
	/// @retval false 失败
	bool sendRequest (void);
	/// @brief 接收超文本传输协议响应
	/// @retval true  成功
	/// @retval false 失败
	bool recvResponse (void);

	/// @brief 获取套接字描述符
	/// @return 套接字描述符
	int sockfd (void) const;

private:
	/// @brief 解析超文本传输协议响应包头
	/// @return 超文本传输协议响应包头
	HttpHeader parseHeader (
		string str ///< [in] 超文本传输协议响应包头字符串
	)	const;

	/// @brief 套接字描述符
	int m_sockfd;
	/// @brief 服务器统一资源定位符
	DnsUrl m_dnsUrl;
};

#endif // _SOCKET_H
