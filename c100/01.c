#include <stdio.h>

int main (void) {
	int i = 0;
	int j = 0;
	int k = 0;
	int num = 0;
	for (i = 1; i <= 4; i++) {
		for (j = 1; j <= 4; j++) {
			for (k = 1; k <= 4; k++) {
				if (i != j && i != k && j != k) {
					num = i*100 + j* 10 + k;
					printf ("Num result: %d\n", num);
				}
			}
		}
	}
	return 0;
}
