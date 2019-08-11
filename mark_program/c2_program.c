#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#if 1
double fun(double x)
{
	double n = 0;
	double s = 0, r = 1.0, v = 1.0, y = 1.0, x2 = x;

	s = 1.0 + 0.5*x;
	for (n = 2; ; n++) {
		x2 *= x; 
		v *= (0.5-n+1);
		y *= n;
		r = (0.5 * v / y) * x2; // fabs(r) == Sn - Sn-1
		s += r;
		if (fabs(r) < 0.000001)
			break;
	}
	printf ("n = %lf\n", n);

	return s;
}
#else
double fun (double x) {
	double i, t=1.0, m=1.0, s=0;
	for (i=1; fabs(m/t)>=0.000001; i++)
	{ 
		s=s+m/t;
		m*=(0.5+1-i)*x;
		t*=i;
		printf ("i=%lf\n", i);
	}
	return s;
}
#endif

int main (void)
{
	double x = 0;
	double s = 0;
	printf ("Enter a double digit:");
	scanf ("%lf", &x);
	s = fun (x);
	printf ("%lf - result is %lf\n", x, s);

	return 0;
}
