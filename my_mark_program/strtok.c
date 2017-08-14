#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define LEN 10

static int strtok_string (char *src_str, char dest_src[][LEN], char *delim)
{
	int i = 0;
	char *p = NULL;
	
	p = strtok (src_str, delim);
	snprintf (dest_src[i], strlen (p)+1, "%s", p);

	while (p = strtok (NULL, delim)) {
		i++;
		snprintf (dest_src[i], strlen (p)+1, "%s", p);
	}

	return 0;
}

int main (void)
{
	char s[] = "111,222,333,aaa,bbb,ccc";
	char *delim = ",";
	char test_buf[LEN][LEN] = {0};
	int i = 0;

	strtok_string (s, test_buf, delim); // test_buf type: char (*)[]
	for (i = 0; i < LEN && strlen (test_buf[i]); i++)
		printf ("test_buf[%d] = %s\n", i, test_buf[i]);
	return 0;
}
