/// @file SaveHTMLToFile.cpp
/// @brief 实现#SaveHTMLToFile类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "SaveHTMLToFile.h"
#include "PluginMngr.h"
#include "Http.h"

SaveHTMLToFile g_pluginSaveHTMLToFile; // 超文本标记语言文件存储插件对象
WebCrawler*    g_app;                  // 应用程序对象

// 插件初始化
// 成功返回true，失败返回false
// 根据超文本标记语言文件存储插件的功能实现基类中的虚函数
bool SaveHTMLToFile::init (
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
// 根据超文本标记语言文件存储插件的功能实现基类中的虚函数
bool SaveHTMLToFile::handler (
	void* arg // [输入/输出] 插件参数
	) {
	// 超文本传输协议响应
	HttpResponse* res = static_cast<HttpResponse*> (arg);

	// 若超文本传输协议响应内容类型不是超文本标记语言
	if (res->m_header.m_contentType.find ("text/html", 0) ==
		string::npos)
		// 返回失败
		return false;

	// 将解析统一资源定位符转换为文件名字符串
	string filename = res->m_dnsUrl.toFilename ();

	// 根据文件名打开超文本标记语言文件输出流
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

	// 将超文本传输协议响应包体写入超文本标记语言文件输出流，若失败
	if (! ofs.write (res->m_body, res->m_len)) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"写入文件%s失败: %s", filename.c_str (), strerror (errno));
		// 关闭超文本标记语言文件输出流
		ofs.close ();
		// 删除超文本标记语言文件
		unlink (filename.c_str ());
		// 返回失败
		return false;
	}

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"文件%s保存成功", filename.c_str ());

	// 关闭超文本标记语言文件输出流
	ofs.close ();
	// 返回成功
	return true;
}
