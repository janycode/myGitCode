#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int main (void)
{
	struct timeval tv; 
	time_t time;
	char str_t[26] = {0};

	gettimeofday (&tv, NULL);

	time = tv.tv_sec + 3600;

	struct tm* p_time = localtime(&time);  
	strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);  

	printf ("%s\n", str_t);

	return 0;
}
