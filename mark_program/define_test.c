#include <stdio.h>

#define DEFINE_VAR()		\
			int a;			\
			unsigned int b; \

#define DEFINE_BREAK(x, y, act)		\
			if (x == y) {			\
				act;				\
			}

typedef struct msg{
	DEFINE_VAR()
	char string[20];
} msg_t;

int main () {
	msg_t mt = {0};
	int i = 0;

	mt.a = 10;
	mt.b = 20;
	for (i=1; i<=99; i++) {
		DEFINE_BREAK(i, mt.a, continue)
		DEFINE_BREAK(i, mt.b, break)
		printf ("i = %d\n", i);
	}

	return 0;
}


