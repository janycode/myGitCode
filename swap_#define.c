/*
 * 函数介绍：字符数组循环右移
 * 输入参数：
 *     @src 字符数组首地址
 *     @n 右移字符个数
 * 输出参数：
 *     @src 字符数组首地址(输入&输出参数)
 * 返回值：void 无
 * 作者：布衣不才b_d
 * 时间：2017.05.04
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void circular_arr (char *src, int n)
{
	int len = strlen (src);
	char *p = (char *)malloc (len + 1);
	char *p_first = p;			// 存储p的首地址

	int offset = len - n;
	char *q = src + offset; 	// q指向字符h -> h
	
	while (n--)
	{
		*p++ = *q++;			// 向p指向的存储空间写入 [hi] 不含\0
	}

	char *d = src;
	while (offset--)
	{
		*p++ = *d++;			// 向p指向的存储空间挨着写入 [abcdefg]
	}

	strcpy (src, p_first);		// src 原字符数组首地址；p_first 循环右移后的字符串首地址
	
	free (p_first);
	p_first = NULL;
}

int main (void)
{
	char str[10] = "abcdefghi";
	printf ("src: %s\n", str);	// >> abcdefghi
	circular_arr (str, 2);
	printf ("dest: %s\n", str);	// >> hiabcdefg
	return 0;
}
