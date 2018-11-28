#include <stdio.h>

int main (void) {
	int i = 0;
	float sum_h = 100;
	float height = sum_h / 2; //初始第一次高度为50

	for (i = 2; i <= 10; i++) { //只需要计算9次
		sum_h += height*2; 
		height /= 2;
	}
	printf ("sum_h = %f, height 10 = %f\n", sum_h, height);
	// 100(1) + 50 + 50(2) + 25 + 25(3) + 17.5 + 17.5(4) + 8.75 + 8.75(5)
	// 4.375 + 4.375(6) + 2.1875 + 2.1875(7) + 1.09375 + 1.09375(8)
	// 0.546875 + 0.546875(9) + 0.2734375 + 0.2734375(10)
	return 0;
}
