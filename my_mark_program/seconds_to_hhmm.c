#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long int timezone_diff = 0;

int main (void)
{
	char time_diff[64] = {0};
	char tmp_time_diff[10] = {0};
	char *p_time_diff = NULL;
	int number = 0;
	int hours = 0;
	int minutes = 0;
	long int seconds_diff = 0;

	scanf ("%s", time_diff);

	/* Change time_diff to number. */
	strncpy (tmp_time_diff, time_diff, strlen (time_diff));
	p_time_diff = &tmp_time_diff[1];
	number = atoi(p_time_diff);
	hours = number / 100;
	printf ("hours = %d\n", hours);
	minutes = number % 100;
	printf ("minutes = %d\n", minutes);

	seconds_diff = (hours * 3600) + (minutes * 60);

	/* Judge + - symbol. */
	if ('+' == tmp_time_diff[0])
	{
		timezone_diff = seconds_diff;
	}
	else if ('-' == tmp_time_diff[0])
	{
		printf ("timezone_diff=%ld\n", timezone_diff);
		printf ("seconds_diff=%ld\n", seconds_diff);
		timezone_diff = 0 - seconds_diff;
		printf ("timezone_diff=%ld\n", timezone_diff);
	}
	else
	{
		printf ("error...\n");
	}
	
	printf ("timezone_diff = %ld\n", timezone_diff);

	return 0;
}

