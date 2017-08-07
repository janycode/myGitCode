/// @file BloomFilter.cpp
/// @brief 实现#BloomFilter类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "BloomFilter.h"

// 构造器
BloomFilter::BloomFilter (void) {
	// 初始化布隆表
	bzero (m_bloomTable, sizeof (m_bloomTable));
}

// 判断某个统一资源定位符是否已经存在
// 存在返回true，否则返回false，同时将其加入布隆表
// 根据布隆过滤器的策略实现基类中的纯虚函数
bool BloomFilter::exist (
	string const& strUrl // [输入] 统一资源定位符
	) {
	// 初始化1位计数器
	int one = 0;

	// 逐个计算哈希值
	for (int i = 0; i < HASH_FUNCS; ++i) {
		// 计算哈希值对应的布隆表位数
		unsigned int bit = hash (i, strUrl) % (sizeof (m_bloomTable) * 8);
		// 计算哈希值对应的布隆表元素下标
		unsigned int idx = bit / (sizeof (m_bloomTable[0]) * 8);
		// 计算哈希值对应的布隆表元素中的位数
		bit %= sizeof (m_bloomTable[0]) * 8;

		// 若此位已为1
		if (m_bloomTable[idx] & 0x80000000 >> bit)
			// 1位计数器加1
			++one;
		// 否则
		else
			// 此位置1
			m_bloomTable[idx] |= 0x80000000 >> bit;
	}

	// 若全部哈希值对应的布隆表位都为1，
	// 则返回true，否则返回false
	return one == HASH_FUNCS;
}

// 用特定的哈希算法计算某个统一资源定位符的哈希值
// 返回32位哈希值
unsigned int BloomFilter::hash (
	int           id,    // [输入] 哈希算法标识号
	string const& strUrl // [输入] 统一资源定位符
	) const {
	// 32位哈希值
	unsigned int val = 0;

	// 若哈希算法标识号...
	switch (id) {
		// 为0
		case 0:
			// 用Times33算法计算参数统一资源
			// 定位符字符串的32位哈希值
			val = m_hash.times33 (strUrl);
			break;

		// 为1
		case 1:
			// 用Times31算法计算参数统一资源
			// 定位符字符串的32位哈希值
			val = m_hash.timesnum (strUrl, 31);
			break;

		// 为2
		case 2:
			// 用AP算法计算参数统一资源
			// 定位符字符串的32位哈希值
			val = m_hash.aphash (strUrl);
			break;

		// 为3
		case 3:
			// 用FNV算法计算参数统一资源
			// 定位符字符串的32位哈希值
			val = m_hash.hash16777619 (strUrl);
			break;

		// 为4
		case 4:
			// 用MySQL算法计算参数统一资源
			// 定位符字符串的32位哈希值
			val = m_hash.mysqlhash (strUrl);
			break;

		// 为5
		case 5:
			// 用循环冗余校验算法计算参数统一资源
			// 定位符字符串的32位哈希值
			val = m_hash.crc32 (strUrl);
			break;

		// 为6
		case 6:
			// 用Times131算法计算参数统一资源
			// 定位符字符串的32位哈希值
			val = m_hash.timesnum (strUrl, 131);
			break;

		// 为7
		case 7:
			// 用Times1313算法计算参数统一资源
			// 定位符字符串的32位哈希值
			val = m_hash.timesnum (strUrl, 1313);
			break;

		// 为其它
		default:
			// 记录一般错误日志
			g_app->m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
				"无效哈希算法标识: %d", id);
	}

	// 返回32位哈希值
	return val;
}
