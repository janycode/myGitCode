/// @file MultiIo.h
/// @brief 声明#MultiIo类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _MULTIIO_H
#define _MULTIIO_H

/// @brief 多路输入输出
class MultiIo {
public:
	/// @brief 构造器
	MultiIo (void);
	/// @brief 析构器
	~MultiIo (void);

	/// @brief 增加需要被关注的输入输出事件
	/// @retval true  成功
	/// @retval false 失败
	bool add (
		int          fd,   ///< [in] 发生输入输出事件的文件描述符
		epoll_event& event ///< [in] 事件描述结构
	)	const;
	/// @brief 删除需要被关注的输入输出事件
	/// @retval true  成功
	/// @retval false 失败
	bool del (
		int          fd,   ///< [in] 发生输入输出事件的文件描述符
		epoll_event& event ///< [in] 事件描述结构
	)	const;
	/// @brief 等待所关注输入输出事件的发生
	/// @retval >0 处于就绪状态的文件描述符数
	/// @retval 0  超时
	/// @retval -1 失败
	int wait (
		epoll_event events[], ///< [out] 事件描述结构数组
		int         max,      ///< [in]  事件描述结构数组容量
		int         timeout   ///< [in]  超时毫秒数，0立即超时，-1无限超时
	)	const;

private:
	/// @brief 多路输入输出对象实例句柄(文件描述符)
	int m_epoll;
};

#endif // _MULTIIO_H
