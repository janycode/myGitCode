#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define LEN 10

static int split_string (char *src_str, char dest_str[][LEN])
{
	int index = 0;
	char *ph = src_str;
	char *p  = src_str;

	if(!src_str)
		return -1;
	/* *p != NULL && *p != 空格 */
	while(*p && isspace(*p) == 0)
	{
		if(index > 9)
			return -1;

		if(*(++p) == ',' || *p == '\0' || isspace(*p))
		{
			memcpy(dest_str[index], ph, (p - ph));
			index++;
			ph = p+1;
			//printf ("dest_str[%d]=%s.\n",index-1,dest_str[index-1]);
		}
	}
	return 0;

}

int main (void)
{
	char *p = "111,222,333,aaa,bbb,ccc";
	char test_buf[LEN][LEN] = {0};
	int i = 0;

	split_string (p, test_buf);
	for (i = 0; i < 10 && strlen (test_buf[i]); i++)
		printf ("test_buf[%d] = %s\n", i, test_buf[i]);
	return 0;
}
