/// @file Thread.h
/// @brief 声明#Thread抽象基类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _THREAD_H
#define _THREAD_H

/// @brief 线程
class Thread {
public:
	/// @brief 析构器
	virtual ~Thread (void) {}

	/// @brief 启动线程
	void start (void);

private:
	/// @brief 线程过程函数
	/// @return 线程返回值
	static void* run (
		void* arg ///< [in,out] 线程参数
	);
	/// @brief 线程处理函数
	/// @return 处理结果
	/// @note 纯虚函数，子类根据不同线程的具体任务给出具体实现
	virtual void* run (void) = 0;

	/// @brief 线程标识
	pthread_t m_tid;
};

#endif // _THREAD_H
