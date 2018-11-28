#include <stdio.h>

int main (void) {
	int r_day = 1;
	int day = 10;
	int r1 = 0;

	while (day--) {
		r_day = (r_day + 1)*2;//第1天的桃子数是第2天桃子数+1后的2倍
		r1 = r_day;
		printf ("%d\n", r1);
		if (day == 1)
			break;
	}
	printf ("r1 = %d\n", r1);

	return 0;
}
