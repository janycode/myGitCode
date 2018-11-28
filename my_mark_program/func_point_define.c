#include <stdio.h>


int add (int a, int b, int c) {
	return a + b + c;	
}

int sub (int a, int b, int c) {
	return a - b - c;
}

int main (void) {
	int (*p_f)(int, int, int);

	int num1 = 10;
	int num2 = 20;
	int num3 = 0;
	int result = 0;

	char symbol = 0;
	printf ("\nPlease input method:");
	scanf ("%c", &symbol);

	printf ("\nPlease input c:");
	scanf ("%d", &num3);


	if (symbol == '+') {
		p_f = add;
	}
	else if (symbol == '-' ){
		p_f = sub;
	} else
		printf ("input method error...\n");

	//可判断参数进行不同的调用，函数指针实现避免多次调用问题
	if (num3 < 10) {
		num3 = 0;
		result = (*p_f)(num1, num2, num3);
	} else {
		result = (*p_f)(num1, num2, num3);
	}

	printf ("num1 %c num2 = %d\n", symbol, result);

	return 0;
}
