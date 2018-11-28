#include <stdio.h>
#include <math.h>

int main (void) {
	int m, i, k, h = 0;
	int leap = 1;

	for (m = 101; m <= 200; m++) {
		k = sqrt(m+1); //10, 11, 12, 13, 14
		for (i = 2; i <= k; i++) { //1不是质数
			if (m % i == 0) {
				leap = 0;
				break;
			}
		}
		if (leap) {
			printf ("%-4d", m);
			h++;
			if (h % 10 == 0) {
				printf ("\n");
			}
		}
		leap = 1;
	}
	printf ("\n The total num is %d\n", h);

	return 0;
}
