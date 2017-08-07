/// @file Thread.cpp
/// @brief 实现#Thread抽象基类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "Thread.h"

// 启动线程
void Thread::start (void) {
	// 线程属性结构
	pthread_attr_t attr;
	// 初始化线程属性结构
	pthread_attr_init (&attr);
	// 设置线程栈空间大小：1M字节
	pthread_attr_setstacksize (&attr, 1024 * 1024);
	// 设置线程分离状态：创建即分离
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

	// 创建线程。线程标识存入成员变量m_tid，线程属性取自先前设
	// 置好的线程属性结构attr，线程过程函数为静态成员函数run，
	// 传递给线程过程函数的参数为指向线程(子类)对象的this指针
	int error = pthread_create (&m_tid, &attr, run, this);
	// 若失败
	if (error)
		// 记录一般错误日志
		g_app->m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
			"pthread_create: %s", strerror (error));

	// 销毁线程属性结构
	pthread_attr_destroy (&attr);
}

// 线程过程函数
// 返回线程返回值
void* Thread::run (
	void* arg // [输入/输出] 线程参数
	) {
	// 通过指向子类对象的基类指针，即创建线程时交给系统内核并由系
	// 统内核回传给线程过程函数的参数arg，调用在线程抽象基类中声
	// 明并为其具体子类所覆盖的虚函数run，执行具体线程的具体任务
	return static_cast<Thread*> (arg)->run ();
}
