#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>

void handle (int signum)
{
	printf ("do something...\n");
}

int main (void)
{
	struct itimerval it;
	int num1 = 1;
	int num2 = 0;
	signal (SIGVTALRM, handle);

	it.it_value.tv_sec = 3;
	it.it_value.tv_usec = 0;

	while (1)
	{
		scanf ("%d", &num2);	
		if (num2 == num1)
		{
			setitimer (ITIMER_VIRTUAL, &it, NULL);
		}
	}
	return 0;
}
