#include <stdio.h>

int main (void) {
	int a,n,count=1;
	long int sn=0,tn=0;

	printf("please input a and n\n");
	scanf("%d %d",&a,&n);
	printf("a=%d,n=%d\n",a,n);
	while(count<=n)
	{
		printf ("sn = %d\n", sn);
		tn=tn+a;
		sn=sn+tn;
		a=a*10;
		++count;
	}
	printf("a+aa+aaa+aaaa+aaaaa = %ld\n",sn);
	return 0;
}
