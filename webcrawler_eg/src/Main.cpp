/// @file Main.cpp
/// @brief 定义#main函数
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "BloomFilter.h"

BloomFilter g_filter;
WebCrawler* g_app = new WebCrawler (g_filter);

/// @brief 显示版本信息
static void showVersion (void) {
	cout << endl;

	cout << "网络爬虫 1.0 版" << endl;
	cout << "版权所有 (C) 2015 达内科技" << endl;

	cout << endl;
}

/// @brief 显示启动画面
static void splashScreen (void) {
	cout << endl;

	// 打开画面文件输入流
	ifstream ifs ("WebCrawler.scr");
	// 若成功
	if (ifs) {
		// 行字符串
		string line;
		// 逐行读取画面文件中的文本
		while (getline (ifs, line))
			// 打印到标准输出
			cout << line << endl;
		// 关闭画面文件输入流
		ifs.close ();
	}

	// 打印空行
	cout << endl;
}

/// @brief 显示使用方法
static void showUsage (
	string const& cmd ///< [in] 启动命令
) {
	cout << endl;

	cout << "用法：" << cmd << " [-v|-d|-h]" << endl;
	cout << "选项：-v - 版权信息" << endl;
	cout << "      -d - 启动精灵" << endl;
	cout << "      -h - 帮助信息" << endl;

	cout << endl;
}

/// @brief 进程入口函数
/// @retval EXIT_SUCCESS 进程成功退出
/// @retval EXIT_FAILURE 进程失败退出
int main (
	int   argc,  ///< [in] 命令行参数个数
	char* argv[] ///< [in] 命令行参数列表
) {
	// 是否以精灵模式运行，初始化为"否"
	bool daemon = false;

	// 解析命令行选项
	char ch;
	while ((ch = getopt (argc, argv, "vdh")) != -1)
		// 若选项...
		switch (ch) {
			// 为v
			case 'v':
				// 显示版本信息
				showVersion ();
				return EXIT_SUCCESS;

			// 为d
			case 'd':
				// 以精灵模式运行
				daemon = true;
				break;

			// 为h或者?
			case 'h':
			case '?':
				// 显示使用方法
				showUsage (argv[0]);
				return EXIT_SUCCESS;

			// 为其它
			default:
				// 显示使用方法
				showUsage (argv[0]);
				return EXIT_FAILURE;
		}

	// 显示启动画面
	splashScreen ();

	// 初始化应用程序对象
	g_app->init (daemon);
	// 执行多路输入输出循环
	g_app->exec ();

	return EXIT_SUCCESS;
}
