/// @file Configurator.h
/// @brief 声明#Configurator类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _CONFIGURATOR_H
#define _CONFIGURATOR_H

/// @brief 配置器
class Configurator {
public:
	/// @brief 构造器
	Configurator (void);

	/// @brief 从指定的配置文件中加载配置信息
	void load (
		string const& cfgFile ///< [in] 配置文件路径
	);

	/// @brief 最低日志等级 \brief
	/// 从低到高依次为：
	/// - 0 - 调试
	/// - 1 - 信息
	/// - 2 - 警告
	/// - 3 - 错误
	/// - 4 - 致命
	/// .
	/// 系统将记录所有不低于指定等级的日志信息
	int m_logLevel;
	/// @brief 日志文件路径
	string m_logFile;
	/// @brief 最大抓取任务数，0表示不抓取，-1表示无限抓取
	int m_maxJobs;
	/// @brief 最大递归深度，种子深度为0，之后逐层递增，-1表示无限深度
	int m_maxDepth;
	/// @brief 原始统一资源定位符队列最大容量，-1表示无限大
	int m_maxRawUrls;
	/// @brief 解析统一资源定位符队列最大容量，-1表示无限大
	int m_maxDnsUrls;
	/// @brief 状态间隔，即状态定时器的周期秒数，0表示不设定时器
	long m_statInterval;
	/// @brief 种子链接，多个链接以逗号隔开
	string m_seeds;
	/// @brief 包含前缀，只抓取带有这些前缀的URL，多个前缀以逗号隔开
	string m_includePrefixes;
	/// @brief 排除前缀，不抓取带有这些前缀的URL，多个前缀以逗号隔开
	string m_excludePrefixes;
	/// @brief 插件路径
	string m_pluginsPath;
	/// @brief 插件列表
	vector<string> m_loadPlugins;
	/// @brief 接受类型
	vector<string> m_acceptTypes;
};

#endif // _CONFIGURATOR_H
