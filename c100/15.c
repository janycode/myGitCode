#include <stdio.h>

int main (void) {
	int score = 0;
	char level;

	printf ("Please input your score:");
	scanf ("%d", &score);

	level = score >= 90 ? 'A' : (score >= 60 ? 'B' : 'C');
	printf ("Your level: %c\n", level);

	return 0;
}
