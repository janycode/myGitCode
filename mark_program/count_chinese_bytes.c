#include <stdio.h>
#include <string.h>

int main (void)
{
	char *p = "你好，世界！";
	char *q1 = "字";
	char *q2 = "abc";

	printf ("p = %s\n", p);
	printf ("strlen(p) = %d\n", strlen (p));
	printf ("strstr("") = %s\n", strstr (p, "好"));

	printf ("-----------------\n");
	printf ("strlen(q1) = %d\n", strlen (q1));
	printf ("strlen(q2) = %d\n", strlen (q2));
	return 0;
}
