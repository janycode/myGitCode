#include <stdio.h>

int main(void) {
	long int i = 0;
	int bon1, bon2, bon4, bon6, bon10, bon;
	bon1 = 10 * 10000 * 0.1;
	bon2 = bon1 + 10 * 10000 * 0.075;
	bon4 = bon2 + 20 * 10000 * 0.05;
	bon6 = bon4 + 20 * 10000 * 0.03;
	bon10 = bon6 + 40 * 10000 * 0.015;
	scanf ("%ld", &i);
	if (i <= 10*10000) {
		bon = i * 0.1;
	} else if (i <= 20*10000) {
		bon = bon1 + (i - 10*10000)*0.075;
	} else if (i <= 40*10000) {
		bon = bon2 + (i - 20*10000)*0.05;
	} else if (i <= 60*10000) {
		bon = bon4 + (i - 40*10000)*0.03;
	} else if (i <= 100*10000) {
		bon = bon6 + (i - 60*10000)*0.015;
	} else {
		bon = bon10 + (i - 100*10000)*0.01;
	}
	printf ("The bon = %d\n", bon);
	return 0;
}
