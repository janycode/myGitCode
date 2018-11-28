#include <stdio.h>

int main () {
	int i = 0;
	int f1;
	int f2;
	f1 = f2 = 1;
	for (i = 1; i <= 20; i++) {
		printf ("%d, %d\n", f1, f2);
		f1 += f2;
		f2 += f1;
	}

	return 0;
}
