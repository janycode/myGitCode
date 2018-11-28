#include <stdio.h>

int main(void) {
	static int k[10] = {0}; // int k[100] = {0}; 当因数超过10个的时候会段错误
	int i, j, n, s;
	for (j = 2; j < 1000; j++) { //1000以内
		n = -1; //保证n每次++后从0索引位置记在k[]里
		s = j;  //判断每一个数
		for (i = 1; i < j; i++) {
			if (j%i == 0) {
				n++;
				s = s - i; //减去因数往下计算
				k[n] = i;
			}
		}
		if (s == 0) {
			printf ("%d is a WanShu!\n", j);
			for (i = 0; i < n; i++) {
				printf ("%d,", k[i]);
				//printf ("%d\n", k[n]);
			}
			printf ("%d\n", k[n]);
		}
		//printf ("j = %d\n", j);
	}
	return 0;
}
