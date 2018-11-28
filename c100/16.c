#include <stdio.h>

int main (void) {
	int a,b,num1,num2,temp;

	printf("please input two numbers:");
	scanf("%d %d",&num1,&num2);

	if(num1<num2)/*交换两个数，使大数放在num1上*/
	{
		temp=num1;
		num1=num2;
		num2=temp;
	}
	a=num1;b=num2;
	while(b!=0)/*利用辗除法，直到b为0为止*/
	{
		temp=a%b;
		a=b;
		b=temp;
	}
	printf ("num1 = %d, num2 = %d\n", num1, num2);
	printf("gongyueshu: %d\n",a);
	printf("gongbeishu: %d\n",num1*num2/a);
	return 0;
}
