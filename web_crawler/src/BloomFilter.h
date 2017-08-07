/// @file BloomFilter.h
/// @brief 声明#BloomFilter类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _BLOOMFILTER_H
#define _BLOOMFILTER_H

#include "UrlFilter.h"
#include "Hash.h"

/// @brief 布隆过滤器
class BloomFilter : public UrlFilter {
public:
	/// @brief 构造器
	BloomFilter (void);

	/// @brief 判断某个统一资源定位符是否已经存在
	/// @retval true  存在
	/// @retval false 不存在，同时将其加入布隆表
	/// @note 根据布隆过滤器的策略实现基类中的纯虚函数
	bool exist (
		string const& strUrl ///< [in] 统一资源定位符
	);

private:
	/// @brief 哈希值数
	static int const HASH_FUNCS = 8;
	/// @brief 布隆表元素数
	static size_t const BLOOMTABLE_SIZE = 1000000;

	/// @brief 用特定的哈希算法计算某个统一资源定位符的哈希值
	/// @return 32位哈希值
	unsigned int hash (
		int           id,    ///< [in] 哈希算法标识号
		string const& strUrl ///< [in] 统一资源定位符
	)	const;

	/// @brief 哈希器
	Hash m_hash;
	/// @brief 布隆表
	unsigned int m_bloomTable[BLOOMTABLE_SIZE];
};

#endif // _BLOOMFILTER_H
