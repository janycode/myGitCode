/// @file WebCrawler.cpp
/// @brief 实现#WebCrawler类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "RecvThread.h"
#include "Socket.h"
#include "StrKit.h"

// 构造器
WebCrawler::WebCrawler (
	UrlFilter& filter // [输入] 统一资源定位符过滤器
	) : m_urlQueues (filter),      // 初始化统一资源定位符队列
	    m_curJobs   (0),           // 初始化当前抓取任务数为0
	    m_start     (time (NULL)), // 初始化启动时间为当前系统时间
	    m_success   (0),           // 初始化成功次数为0
	    m_failure   (0) {          // 初始化失败次数为0
	// 初始化互斥锁
	pthread_mutex_init (&m_mutex, NULL);
	// 初始化条件变量
	pthread_cond_init (&m_cond, NULL);
}

// 析构器
WebCrawler::~WebCrawler (void) {
	// 销毁条件变量
	pthread_cond_destroy (&m_cond);
	// 销毁互斥锁
	pthread_mutex_destroy (&m_mutex);
}

// 初始化
void WebCrawler::init (
	bool daemon /* = false */ // [输入] 是否以精灵进程方式运行
	) {
	// 通过配置器从指定的配置文件中加载配置信息
	m_cfg.load ("WebCrawler.cfg");

	// 若以精灵进程方式运行
	if (daemon)
		// 使调用进程成为精灵进程
		initDaemon ();

	// 通过插件管理器加载插件
	m_pluginMngr.load ();

	// 若切换工作目录至下载目录失败
	if (chdir ("../download") == -1)
		// 记录一般错误日志
		m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
			"chdir: %s", strerror (errno));

	// 初始化最大文件描述符数为1K
	initMaxFiles (1024);
	// 将种子链接压入原始统一资源定位符队列
	initSeeds ();
	// 启动域名解析线程
	initDns ();
	// 启动状态定时器
	initTicker ();
	// 启动发送线程
	initSend ();
}

// 执行多路输入输出循环
void WebCrawler::exec (void) {
	// 无限循环
	for (;;) {
		// 输入输出事件结构数组
		epoll_event events[10];
		// 等待所关注输入输出事件的发生，两秒超时
		int fds = m_multiIo.wait (events, sizeof (events) /
			sizeof (events[0]), 2000);

		// 若超时或被信号中断
		if (fds <= 0) {
			// 若没有抓取任务且原始统一资源定位符
			// 队列和解析统一资源定位符队列都为空
			if (! m_curJobs &&
				m_urlQueues.emptyRawUrl () &&
				m_urlQueues.emptyDnsUrl ()) {

				// 等一秒
				sleep (1);

				// 若没有抓取任务且原始统一资源定位符
				// 队列和解析统一资源定位符队列都为空
				if (! m_curJobs &&
					m_urlQueues.emptyRawUrl () &&
					m_urlQueues.emptyDnsUrl ())
					// 抓取任务完成，跳出循环
					break;
			}
			// 否则
			else
			// 若抓取任务到限且原始统一资源定位符
			// 队列和解析统一资源定位符队列都为满
			if (m_curJobs == m_cfg.m_maxJobs &&
				m_urlQueues.fullRawUrl () &&
				m_urlQueues.fullDnsUrl ()) {

				// 等一秒
				sleep (1);

				// 若抓取任务到限且原始统一资源定位符
				// 队列和解析统一资源定位符队列都为满
				if (m_curJobs == m_cfg.m_maxJobs &&
					m_urlQueues.fullRawUrl () &&
					m_urlQueues.fullDnsUrl ())
					// 清空原始统一资源定位符队列，避免死锁
					m_urlQueues.clearRawUrl ();
			}
		}

		// 依次处理每个处于就绪状态的文件描述符
		for (int i = 0; i < fds; ++i) {
			// 从事件结构中取出套接字对象指针
			Socket* socket = (Socket*)events[i].data.ptr;

			// 若为异常事件
			if (events[i].events & EPOLLERR ||
				events[i].events & EPOLLHUP ||
				! (events[i].events & EPOLLIN)) {
				// 记录警告日志
				m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
					"套接字异常");
				// 删除需要被关注的输入输出事件
				m_multiIo.del (socket->sockfd (), events[i]);
				// 销毁套接字对象
				delete socket;
				// 继续下一轮循环
				continue;
			}

			// 删除需要被关注的输入输出事件
			m_multiIo.del (socket->sockfd (), events[i]);

			// 记录调试日志
			m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
				"套接字%d上有数据可读，创建接收线程接收数据",
				socket->sockfd ());

			// 创建接收线程对象并启动接收线程
			(new RecvThread (socket))->start ();
		}
	}

	// 记录调试日志
	m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__, "任务完成");
}

// 启动一个抓取任务
void WebCrawler::startJob (void) {
	// 加锁互斥锁
	pthread_mutex_lock (&m_mutex);

	// 若当前抓取任务数到限
	while (m_curJobs == m_cfg.m_maxJobs)
		// 等待条件变量
		pthread_cond_wait (&m_cond, &m_mutex);

	// 从解析统一资源定位符队列弹出解析统一资源定位符
	DnsUrl dnsUrl = m_urlQueues.popDnsUrl ();

	// 用所弹出解析统一资源定位符创建套接字对象
	Socket* socket = new Socket (dnsUrl);
	// 若通过套接字对象发送超文本传输协议请求成功
	if (socket->sendRequest ())
		// 当前抓取任务数加一
		++m_curJobs;
	// 否则
	else
		// 销毁套接字对象
		delete socket;

	// 解锁互斥锁
	pthread_mutex_unlock (&m_mutex);
}

// 停止一个抓取任务
void WebCrawler::stopJob (
	bool success /* = true */ // [输入] 是否成功
	) {
	// 加锁互斥锁
	pthread_mutex_lock (&m_mutex);

	// 当前抓取任务数减一，若其由到限变为未到限
	if (--m_curJobs == m_cfg.m_maxJobs - 1)
		// 唤醒等待条件变量的线程
		pthread_cond_signal (&m_cond);

	// 若成功抓取
	if (success)
		// 成功次数加一
		++m_success;
	// 否则
	else
		// 失败次数加一
		++m_failure;

	// 解锁互斥锁
	pthread_mutex_unlock (&m_mutex);
}

// SIGALRM(14)信号处理
void WebCrawler::sigalrm (
	int signum // [输入] 信号编号
	) {
	// 抓取时间 = 当前时间 - 启动时间
	time_t t = time (NULL) - g_app->m_start;

	// 记录信息日志
	g_app->m_log.printf (Log::LEVEL_INF, __FILE__, __LINE__,
		"当前任务  原始队列  解析队列  抓取时间  成功次数  失败次数  成功率\n"
		"------------------------------------------------------------------\n"
		"%8d  %8u  %8u  %02d:%02d:%02d  %8u  %8u  %5u%%",
		g_app->m_curJobs,
		g_app->m_urlQueues.sizeRawUrl (),
		g_app->m_urlQueues.sizeDnsUrl (),
		t / 3600, t % 3600 / 60, t % 60,
		g_app->m_success,
		g_app->m_failure,
		g_app->m_success ? g_app->m_success * 100 /
			(g_app->m_success + g_app->m_failure) : 0);
}

// 使调用进程成为精灵进程
void WebCrawler::initDaemon (void) const {
	// 创建子进程
	pid_t pid = fork ();

	// 若失败
	if (pid == -1)
		// 记录一般错误日志
		m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
			"fork: %s", strerror (errno));

	// 若为父进程
	if (pid)
		// 退出，使子进程成为孤儿进程并被init进程收养
		exit (EXIT_SUCCESS);

	// 子进程创建新会话并成为新会话中唯一进程组的组长
	// 进程，进而与原会话、原进程组和控制终端脱离关系
	setsid ();

	// 打开空设备文件
	int fd = open("/dev/null", O_RDWR, 0);
	// 若成功
	if (fd != -1) {
		// 复制空设备文件描述符到标准输入
		dup2 (fd, STDIN_FILENO);
		// 复制空设备文件描述符到标准输出
		dup2 (fd, STDOUT_FILENO);
		// 复制空设备文件描述符到标准出错
		dup2 (fd, STDERR_FILENO);
		// 若空设备文件描述符大于标准出错
		if (fd > STDERR_FILENO)
			// 关闭空设备文件描述符
			close (fd);
	}

	// 若配置器中的日志文件路径非空且打开(创建)日志文件成功
	if (! m_cfg.m_logFile.empty () &&
		(fd = open (m_cfg.m_logFile.c_str (),
		O_WRONLY | O_APPEND | O_CREAT, 0664)) != -1) {
		// 复制日志文件描述符到标准输出
		dup2 (fd, STDOUT_FILENO);
		// 若日志文件描述符大于标准出错
		if (fd > STDERR_FILENO)
			// 关闭日志文件描述符
			close (fd);
	}
}

// 初始化最大文件描述符数
// 成功返回true，失败返回false
bool WebCrawler::initMaxFiles (
	rlim_t maxFiles // [输入] 最大文件描述符数
	) const {
	// 资源限制结构
	rlimit rl;

	// 若获取当前进程可同时打开的最大文件数失败
	if (getrlimit (RLIMIT_NOFILE, &rl) == -1) {
		// 记录警告日志
		m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"getrlimit: %s", strerror (errno));
		// 返回失败
		return false;
	}

	// 若最大文件描述符数超过系统允许的极限
	if (maxFiles > rl.rlim_max) {
		// 记录警告日志
		m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"文件描述符上限不能超过%lu", rl.rlim_max);
		// 返回失败
		return false;
	}

	// 若设置当前进程可同时打开的最大文件数失败
	rl.rlim_cur = maxFiles;
	if (setrlimit (RLIMIT_NOFILE, &rl) == -1) {
		// 记录警告日志
		m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"setrlimit: %s", strerror (errno));
		// 返回失败
		return false;
	}

	// 返回成功
	return true;
}

// 将种子链接压入原始统一资源定位符队列
void WebCrawler::initSeeds (void) {
	// 若配置器中的种子链接为空
	if (m_cfg.m_seeds.empty ())
		// 记录一般错误日志
		m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
			"没种子咋干活嘞？");

	// 拆分种子链接字符串，以逗号为分隔符，不限拆分次数
	vector<string> seeds = StrKit::split (m_cfg.m_seeds, ",", 0);

	// 依次处理每个种子链接
	for (vector<string>::iterator it = seeds.begin ();
		it != seeds.end (); ++it)
		// 若规格化成功
		if (RawUrl::normalized (*it))
			// 压入原始统一资源定位符队列
			m_urlQueues.pushRawUrl (RawUrl (*it));
}

// 启动域名解析线程
void WebCrawler::initDns (void) {
	// 通过域名解析线程对象启动域名解析线程
	m_dnsThread.start ();
	/*
	// 若解析统一资源定位符队列为空，延时再查，最多查八次
	for (int try = 1; m_urlQueues.emptyDnsUrl () && try < 8;
		usleep (10000 << try++));

	// 若解析统一资源定位符队列为空
	if (m_urlQueues.emptyDnsUrl ())
		// 记录一般错误日志
		m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
			"解析URL队列空");
	*/
}

// 启动状态定时器
void WebCrawler::initTicker (void) const {
	// 若配置器中的状态间隔有效
	if (m_cfg.m_statInterval > 0) {
		// 若设置SIGALRM(14)信号处理失败
		if (signal (SIGALRM, sigalrm) == SIG_ERR)
		/*
		struct sigaction act = {};
		act.sa_handler = sigalrm;
		act.sa_flags = SA_RESTART;
		if (sigaction (SIGALRM, &act, NULL) == -1)
		*/
			// 记录一般错误日志
			m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
				"signal: %s", strerror (errno));

		// 间隔时间
		itimerval it;
		// 初始间隔(秒)
		it.it_value.tv_sec = m_cfg.m_statInterval;
		// 初始间隔(微秒)
		it.it_value.tv_usec = 0;
		// 重复间隔(秒)
		it.it_interval.tv_sec = m_cfg.m_statInterval;
		// 重复间隔(微秒)
		it.it_interval.tv_usec = 0;

		// 若设置真实间隔定时器失败
		if (setitimer (ITIMER_REAL, &it, NULL) == -1)
			// 记录一般错误日志
			m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
				"setitimer: %s", strerror (errno));
	}
}

// 启动发送线程
void WebCrawler::initSend (void) {
	// 通过发送线程对象启动发送线程
	m_sendThread.start ();
}
