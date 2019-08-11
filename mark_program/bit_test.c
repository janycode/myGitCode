/* 
 * 使用bit位作为掩码来实现计数累加，和对计数累加的判断，限制计数是limit=100
 */

#include <stdio.h>
#include <stdlib.h>

#define MASK_BIT(val, bit) ((val)[((bit) / 32)] & (1 << ((bit) % 32)))

typedef struct {
	unsigned int id_mask[4];
} data_t;

static data_t g_data_t = {0};
static long test_value = 0;

void func_bit (int *p_id, int limit) {
	int cnt = 1;

	printf ("while: cnt = %d ", cnt);
	while (MASK_BIT(g_data_t.id_mask, cnt)) {
		cnt++;
		if (cnt >= limit) {
			break;
		}
		printf ("%d ", cnt);
	}
	printf ("\n\n");

	g_data_t.id_mask[(cnt / 32) % 4] |= (1 << (cnt % 32));
	*p_id = cnt;

	printf ("func: cnt = %d\n", cnt);
	printf ("func: id_mask[0] = 0x%X\n", g_data_t.id_mask[0]);
	printf ("func: id_mask[1] = 0x%X\n", g_data_t.id_mask[1]);
	printf ("func: id_mask[2] = 0x%X\n", g_data_t.id_mask[2]);
	printf ("func: id_mask[3] = 0x%X\n", g_data_t.id_mask[3]);
	printf ("\n");
}

int main (int argc, char *argv[]) {
	int id = 0;
	int *p = NULL;
	int i = 0;
	int index = 0;
    int limit = 100;

	p = &id;
	printf ("\ninit test_value = %ld\n", test_value);

	index = atoi(argv[1]);
	printf ("argv[1] = %d\n", index);
	printf ("loop: i= ");
	for (i = 0; i < index; i++) {
		g_data_t.id_mask[(i / 32) % 4] |= (1 << i);
		printf ("%d ", i);
	}
	printf ("\n");

	func_bit (p, limit);
	printf ("dest: id = %d\n", id);
	printf ("end : test_value = %ld\n", test_value);
#if 0
	short a = 0x01;
	char c = *(char *)&a;
	if (c == 0x1)
		printf ("small port\n");
	else
		printf ("big port\n");
#endif
	return 0;
}
