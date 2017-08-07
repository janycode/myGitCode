/// @file Hash.h
/// @brief 声明#Hash类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _HASH_H
#define _HASH_H

/// @brief 哈希器
class Hash {
public:
	/// @brief 构造器
	Hash (void);

	/// @brief Times33哈希算法
	/// @return 参数字符串的32位哈希值
	/// @remark  hash(i) = hash(i-1)×33+str[i] (hash(-1) = 0)
	unsigned int times33 (
		string const& str ///< [in] 被哈希字符串
	)	const;
	/// @brief TimesN哈希算法
	/// @return 参数字符串的32位哈希值
	/// @remark  hash(i) = hash(i-1)×N+str[i] (hash(-1) = 0)
	unsigned int timesnum (
		string const& str, ///< [in] 被哈希字符串
		int           num  ///< [in] N
	)	const;
	/// @brief AP哈希算法
	/// @return 参数字符串的32位哈希值
	unsigned int aphash (
		string const& str ///< [in] 被哈希字符串
	)	const;
	/// @brief FNV哈希算法
	/// @return 参数字符串的32位哈希值
	unsigned int hash16777619 (
		string const& str ///< [in] 被哈希字符串
	)	const;
	/// @brief MySQL哈希算法
	/// @return 参数字符串的32位哈希值
	unsigned int mysqlhash (
		string const& str ///< [in] 被哈希字符串
	)	const;
	/// @brief 循环冗余校验算法
	/// @return 参数字符串的循环冗余校验码
	unsigned int crc32 (
		string const& str ///< [in] 被校验字符串
	)	const;

private:
	/// @brief CRC表
	unsigned int m_crcTable[256];
};

#endif // _HASH_H
