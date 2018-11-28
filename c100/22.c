#include <stdio.h>

int main (void) {
	char team[3] = {'x', 'y', 'z'};
	int a, b, c; //a b c对应对手
	int i, j, k;

	for (i = 0; i <= 2; i++) {
		a = team[i];
		for (j = 0; j <= 2; j++) {
			b = team[j];
			if (i != j) {
				for (k = 0; k <= 2; k++) {
					c = team[k];
					if (i != k && j != k) {
						if (a != team[0] && c != team[0] && c != team[2])
							printf ("Play Team: a:%c, b:%c, c:%c\n", a, b, c);
					}
				}
			}
		}
	}

	return 0;
}
