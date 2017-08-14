#include <stdio.h>

int main (void)
{
	char *message;
	message = "hello, world!";
	
	printf ("1111\n");
	printf ("message=%s\n", message);
	printf ("2222\n");

	char *aa;
	aa = 'c';
	printf ("3333, aa=%c, aa=%x\n", aa, aa);

	return 0;
}
