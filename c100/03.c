#include <stdio.h>
#include <math.h>

int main (void) {
	long int i, x, y, z;

	for (i = 1; i < 10000; i++) {
		x = sqrt (i + 100); // sqrt() 开平方，返回结果
		y = sqrt (i + 268);
		if (x*x == i+100 && y*y == i+268)
			printf ("num is %ld\n", i);
	}
	return 0;
}
/* gcc test.c -o test -lm */