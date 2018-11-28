#include <stdio.h>

int main (void) {
	int num, i;

	scanf ("%d", &num);

	for (i = 2; i <= num; i++) {
		while (num != i) {
			if (num % i == 0) { //n能整除i
				printf ("%d * ", i);
				num = num/i; //n继续往下除
			} else
				break;
		}
	}
	printf ("%d\n", num);

	return 0;
}
