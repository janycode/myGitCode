#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
	int r = 0;
	char buf[4096] = {0};

	while ((r = read (STDIN_FILENO, buf, sizeof (buf))) > 0)
	{
		if (write (STDOUT_FILENO, buf, r) != r)
		{
			printf ("write error\n");
		}
	}
	if (r < 0)
		printf ("read error\n");

	return 0;
}
