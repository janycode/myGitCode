#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void str_trim(char *a)
{
	int len = strlen(a);
	int i=0;
	char *p = NULL;
	char *q = NULL;

	if (len <= 0) return;

	
	
	p = (char *)malloc (len+1);
	q = p;

	strncpy (p, a+1, len-2);
	a = p;
	printf ("a=%s\n", a);

#if 0
	while (*a && i < len) {
		printf ("*p = %c\n", *p);
		*p++ = a + i+ 1;
	}

	while (*p) *a++ = *p++;
#endif
	free(q);
}

int main ()
{
	char str[20] = {0};
	char *p = "aaaaa";

	strcpy (str, "\"abc\""); // "abc"

	str_trim(str);
	printf ("str=%s\n", str);
	printf ("p=%s\n", p);
	return 0;
}
