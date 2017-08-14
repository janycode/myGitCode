#include <stdio.h>

int count_one_bits (unsigned int value)
{
	int ones;

	for (ones = 0; value != 0; value = value >> 1) {
		/* 如果最低位的值为1，计数增1 */
		if (value % 2 != 0)
			ones++;
	}

	return ones;
}

int main (void)
{
	int i = 0x10011001;
	int count = 0;

	printf ("i = %x\n", i);
	
	count = count_one_bits (i);
	printf ("i的二进制中bit1的个数为：%d个。\n", count);
	return 0;
}
