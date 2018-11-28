#include <stdio.h>

int main (void) {
	int i, j, k = 0;
	int num = 0;

	for (num = 2; num < 1000; num++) {
		i = num % 10;
		j = num / 10 % 10;
		k = num / 100;
		if ((i + j*10 + k*100) == (i*i*i + j*j*j + k*k*k)) {
			printf ("%d ", num);
		}
	}
	printf ("\n");
	return 0;
}
