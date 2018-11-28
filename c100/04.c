#include <stdio.h>

int main (void) {
	int year, month, day;
	int sum_day = 0;

	printf ("Please input[year month day]: "); 
	scanf ("%d %d %d", &year, &month, &day);

	// 一三五七八十腊，三十一天用不差；四六九冬三十天，平年二月二十八。
	switch(month) {
		case 1:
			sum_day = 0; 
			break;
		case 2:
			sum_day = 28;
			break;
		case 3:
			sum_day = 28+31;
			break;
		case 4:
			sum_day = 28+31+30;
			break;
		case 5:
			sum_day = 28+31+30+31;
			break;
		case 6:
			sum_day = 28+31+30+31+30;
			break;
		case 7:
			sum_day = 28+31+30+31+30+31;
			break;
		case 8:
			sum_day = 28+31+30+31+30+31+31;
			break;
		case 9:
			sum_day = 28+31+30+31+30+31+31+30;
			break;
		case 10:
			sum_day = 28+31+30+31+30+31+31+30+31;
			break;
		case 11:
			sum_day = 28+31+30+31+30+31+31+30+31+30;
			break;
		case 12:
			sum_day = 28+31+30+31+30+31+31+30+31+30+31;
			break;
		default:
			printf ("The month is error, please check!\n");
			break;
	}
	sum_day += day;
	// 闰年？2月29天
	if (year%400 == 0 || (year%4 == 0 && year%100 != 0)) {
		printf ("今年是闰年，2月29天！\n");
		if (month >= 2) {
			sum_day += 1;
		}
	}
	printf ("截止输入的日期当年已过了 %d 天。\n", sum_day);
	return 0;
}
