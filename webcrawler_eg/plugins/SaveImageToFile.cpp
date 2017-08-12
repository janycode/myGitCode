/// @file SaveImageToFile.cpp
/// @brief 实现#SaveImageToFile类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "SaveImageToFile.h"
#include "PluginMngr.h"
#include "Http.h"

SaveImageToFile g_pluginSaveImageToFile; // 图像文件存储插件对象
WebCrawler*     g_app;                   // 应用程序对象

// 插件初始化
// 成功返回true，失败返回false
// 根据图像文件存储插件的功能实现基类中的虚函数
bool SaveImageToFile::init (
	WebCrawler* app // [输入/输出] 应用程序对象
	) {
	// 以超文本标记语言插件的身份
	// 注册到应用程序对象的插件管理器中
	(g_app = app)->m_pluginMngr.registerHtmlPlugin (this);

	// 返回成功
	return true;
}

// 插件处理
// 成功返回true，失败返回false
// 根据图像文件存储插件的功能实现基类中的虚函数
bool SaveImageToFile::handler (
	void* arg // [输入/输出] 插件参数
	) {
	// 超文本传输协议响应
	HttpResponse* res = static_cast<HttpResponse*> (arg);

	// 若超文本传输协议响应内容类型是超文本标记语言
	if (res->m_header.m_contentType.find ("text/html", 0) !=
		string::npos) {
		// 正则表达式
		regex_t ex;

		// 编译正则表达式：<img [^>]*src="\s*\([^ >"]*\)\s*"
		//     \s - 匹配任意空白字符(空格、制表、换页等)
		//      * - 重复前一个匹配项任意次
		//     \( - 子表达式左边界
		//     \) - 子表达式右边界
		// [^ >"] - 匹配任意不是空格、大于号和双引号的字符
		int error = regcomp (&ex,
			"<img [^>]*src=\"\\s*\\([^ >\"]*\\)\\s*\"", 0);
		// 若失败
		if (error) {
			// 错误信息缓冲区
			char errInfo[1024];
			// 获取正则表达式编译错误信息
			regerror (error, &ex, errInfo, sizeof (errInfo) /
				sizeof (errInfo[0]));
			// 记录一般错误日志
			g_app->m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
				"regcomp: %s", errInfo);
		}

		// 超文本标记语言页面内容字符串
		char const* html = res->m_body;
		// 匹配集合
		regmatch_t match[2];

		// 在超文本标记语言页面内容字符串中，
		// 查找所有与正则表达式匹配的内容
		while (regexec (&ex, html, sizeof (match) /
			sizeof (match[0]), match, 0) != REG_NOMATCH) {
			// regex : <img [^>]*src="\s*\([^ >"]*\)\s*"
			// html  : ...<img src="  /images/potalaka.jpg  "...
			//            |           |<----match[1]---->|  |
			//            |         rm_so              rm_eo|
			//            |<--------------match[0]--------->|
			//          rm_so                             rm_eo

			// 匹配子表达式的内容首地址
			html += match[1].rm_so;
			// 匹配子表达式的内容字符数
			size_t len = match[1].rm_eo - match[1].rm_so;
			// 匹配子表达式的内容字符串，即图像源中的统一资源定位符
			string strUrl (html, len);
			// 移至匹配主表达式的内容后，以备在下一轮循环中继续查找
			html += len + match[0].rm_eo - match[1].rm_eo;

			// 若添加域名失败
			if (! res->m_dnsUrl.attachDomain (strUrl))
				// 继续下一轮循环
				continue;

			// 记录调试日志
			g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
				"抽取到一个深度为%d的统一资源定位符\"%s\"",
				res->m_dnsUrl.m_depth, strUrl.c_str ());

			// 若规格化失败
			if (! RawUrl::normalized (strUrl)) {
				// 记录警告日志
				g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
					"规格化统一资源定位符\"%s\"失败", strUrl.c_str ());
				// 继续下一轮循环
				continue;
			}

			// 压入原始统一资源定位符队列
			g_app->m_urlQueues.pushRawUrl (RawUrl (strUrl,
				RawUrl::ETYPE_IMAGE, res->m_dnsUrl.m_depth));
		}

		// 释放正则表达式
		regfree (&ex);
	}
	// 否则，若超文本传输协议响应内容类型是图像
	else if (res->m_header.m_contentType.find (
		"image", 0) != string::npos) {
		// 将解析统一资源定位符转换为文件名字符串
		string filename = res->m_dnsUrl.toFilename ();

		// 根据文件名打开图像文件输出流
		ofstream ofs (filename.c_str (), ios::binary);
		// 若失败
		if (! ofs) {
			// 记录警告日志
			g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
				"打开文件%s失败: %s", filename.c_str (),
				strerror (errno));
			// 返回失败
			return false;
		}

		// 将超文本传输协议响应包体写入图像文件输出流，若失败
		if (! ofs.write (res->m_body, res->m_len)) {
			// 记录警告日志
			g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
				"写入文件%s失败: %s", filename.c_str (), strerror (errno));
			// 关闭图像文件输出流
			ofs.close ();
			// 删除图像文件
			unlink (filename.c_str ());
			// 返回失败
			return false;
		}

		// 记录调试日志
		g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
			"文件%s保存成功", filename.c_str ());

		// 关闭图像文件输出流
		ofs.close ();
	}

	// 返回成功
	return true;
}
