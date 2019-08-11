#include <stdio.h>

int main (void)
{
	char ch;

	while ((ch = getchar ()) != EOF) {
		if (ch < '0' || ch > '9') 
			continue;
		putchar (ch);
	}
	return 0;
}
