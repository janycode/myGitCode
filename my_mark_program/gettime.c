#include <stdio.h>
#include <string.h>
#include <sys/time.h>

//#define BEIJINGTIME		8
#define WORLDTIME		1

#define DAY				(60*60*24)
#define YEARFIRST		2001
#define YEARSTART		(365*(YEARFIRST-1970) + 8)
#define YEAR400			(365*4*100 + (4*(100/4 - 1) + 1))
#define YEAR100			(365*100 + (100/4 - 1))
#define YEAR004			(365*4 + 1)
#define YEAR001			365

/* 取当前时间，精确到微秒 */

int main (void)
{
	struct timeval tv;
	long sec, usec;
	int yy, mm, dd, hh, mi, ss, ms;
	int ad;
	int y400, y100, y004, y001;
	int m[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int i;

	memset(&tv, 0, sizeof(tv));
	gettimeofday (&tv, NULL);
	sec = tv.tv_sec;
	usec = tv.tv_usec;

	//sec = sec + (60*60) * BEIJINGTIME;
	sec = sec + (60*60) * WORLDTIME;
	ad = sec / DAY;
	ad = ad - YEARSTART;  
	y400 = ad / YEAR400;
	y100 = (ad - y400*YEAR400) / YEAR100;
	y004 = (ad - y400*YEAR400 - y100*YEAR100) / YEAR004;
	y001 = (ad - y400*YEAR400 - y100*YEAR100 - y004*YEAR004) / YEAR001;
	// 年
	yy = y400*4*100 + y100*100 + y004*4 + y001*1 + YEARFIRST; 
	dd = (ad - y400*YEAR400 - y100*YEAR100 - y004*YEAR004) % YEAR001; 

	//月
	if(0 == yy % 1000)
	{
		if(0 == (yy / 1000) % 4)
		{
			m[1] = 29;
		}
	}
	else
	{
		if(0 == yy % 4)
		{
			m[1] = 29;
		}
	}
	for(i = 1; i <= 12; i++)
	{
		if(dd - m[i] < 0)
		{
			break;
		}
		else
		{
			dd = dd - m[i]; // 142-31-28-31-30-31(==-9)-30-31-31-30-31-30-31
		}
	}
	dd = dd + 1;
	// 日
	mm = i;
	// 小时
	hh = sec/(60*60) % 24;
	// 分
	mi = sec/60 - sec/(60*60)*60;
	// 秒
	ss = sec - sec/60*60;
	// 毫秒
	ms = usec;

	printf("%d-%02d-%02d %02d:%02d:%02d : %06d\n", yy, mm, dd, hh, mi, ss, ms);

	return 0;
}
