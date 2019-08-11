#include <stdio.h>
#include <unistd.h>
#include <signal.h>

typedef void (*sighandler_t)(int);

void someone_func_handler(int param)
{
	printf("hello\n");
}

int main(void)
{
	int i;
	sighandler_t handler = someone_func_handler;

	signal(SIGALRM, handler);
	
	alarm(5);
	
	for(i = 1; i < 7; i++)
	{
		printf("sleep %d ...\n", i);
		sleep(1);
	}
	return 0;
}
