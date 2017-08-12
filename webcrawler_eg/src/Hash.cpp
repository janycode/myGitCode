/// @file Hash.cpp
/// @brief 实现#Hash类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "Hash.h"

// 构造器
Hash::Hash (void) {
	// 初始化CRC表
	for (unsigned int i = 0; i < sizeof (m_crcTable) /
		sizeof (m_crcTable[0]); ++i) {
		unsigned int crc = i;

		for (unsigned j = 0; j < 8; ++j)
			if (crc & 1)
				crc = crc >> 1 ^ 0xEDB88320;
			else
				crc = crc >> 1;

		m_crcTable[i] = crc;
	}
}

// Times33哈希算法
// 返回参数字符串的32位哈希值
// hash(i) = hash(i-1)×33+str[i] (hash(-1) = 0)
unsigned int Hash::times33 (
	string const& str // [输入] 被哈希字符串
	) const {
	unsigned int val = 0;

	size_t len = str.size ();
	for (size_t i = 0; i < len; ++i)
		val = (val << 5) + val + (unsigned char)str[i];

	return val;
}

// TimesN哈希算法
// 返回参数字符串的32位哈希值
// hash(i) = hash(i-1)×N+str[i] (hash(-1) = 0)
unsigned int Hash::timesnum (
	string const& str, // [输入] 被哈希字符串
	int           num  // [输入] N
	) const {
	unsigned int val = 0;

	size_t len = str.size ();
	for (size_t i = 0; i < len; ++i)
		val = val * num + (unsigned char)str[i];

	return val;
}

// AP哈希算法
// 返回参数字符串的32位哈希值
unsigned int Hash::aphash (
	string const& str // [输入] 被哈希字符串
	) const {
	unsigned int val = 0;

	size_t len = str.size ();
	for (size_t i = 0; i < len; ++i)
		if (i & 1)
			val ^= ~(val << 11 ^ (unsigned char)str[i] ^ val >> 5);
		else
			val ^= val << 7 ^ (unsigned char)str[i] ^ val >> 3;

	return val & 0x7FFFFFFF;
}

// FNV哈希算法
// 返回参数字符串的32位哈希值
unsigned int Hash::hash16777619 (
	string const& str // [输入] 被哈希字符串
	) const {
	unsigned int val = 0;

	size_t len = str.size ();
	for (size_t i = 0; i < len; ++i) {
		val *= 16777619;
		val ^= (unsigned char)str[i];
	}

	return val;
}

// MySQL哈希算法
// 返回参数字符串的32位哈希值
unsigned int Hash::mysqlhash (
	string const& str // [输入] 被哈希字符串
	) const {
	unsigned int nr1 = 1, nr2 = 4;

	size_t len = str.size ();
	for (size_t i = 0; i < len; ++i) {
		nr1 ^= ((nr1 & 63) + nr2) * (unsigned char)str[i] + (nr1 << 8);
		nr2 += 3;
	}

	return nr1;
}

// 循环冗余校验算法
// 返回参数字符串的循环冗余校验码
unsigned int Hash::crc32 (
	string const& str // [输入] 被校验字符串
	) const {
	unsigned int val = 0xFFFFFFFF;

	size_t len = str.size ();
	for (size_t i = 0; i < len; ++i)
		val = m_crcTable[(val & 0xFF) ^ (unsigned char)str[i]] ^ val >> 8;

	return ~val;
}
