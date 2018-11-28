#include <stdio.h>

int main (void) {
	int k, i, j;
	i = j = 0;
	int a[4][4] = {0};
	k = 1;

	int n = 4;

	for (i = 0; i < n; i++) {
		if (i % 2 == 0) {
			for (j = 0; j <= i; j++)
				a[i][j] = k++;
			for (j = i-1; j >= 0; j--)
				a[j][i] = k++;
		} else {
			for (j = 0; j <= i; j++)
				a[j][i] = k++;
			for (j = i - 1; j >= 0; j--)
				a[i][j] = k++;
		}
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
			printf ("\ta[%d][%d] = %d", i, j, a[i][j]);
		printf ("\n");
	}

	return 0;
}
