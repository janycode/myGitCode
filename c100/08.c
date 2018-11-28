#include <stdio.h>

int main (void) {
	int i = 0, j = 0;
	int result = 0;

	for (i = 1; i <= 9; i++) {
		for (j = 1; j <= i; j++) {
			result = i*j;
			printf ("%d*%d = %-3d", j, i, result);
		}
		printf ("\n");
	}
	
	return 0;
}
