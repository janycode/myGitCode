#include <stdio.h>

typedef enum my_enum {
	e_a = 1,
	e_b = 2,
	e_c = 3,
	e_d = 4,
	e_e = 5
} e_my_enum_t;

typedef struct my_struct {
	e_my_enum_t enu_n;
	int			int_a;
	float		flo_b;
	double		dou_c;
} my_struct_t;

int main()
{
	int i = 0, j = 0;
	int *p = NULL;
	my_struct_t test_t = {0};
	char* elem_string[4] = {"enu_n", "int_a", "flo_b", "dou_c"}; 
	int elem_length[4] = {sizeof(test_t.enu_n), sizeof(test_t.int_a), sizeof(test_t.flo_b), sizeof(test_t.dou_c)};


	test_t.enu_n = e_c;
	test_t.int_a = 11;
	test_t.flo_b = 1.25;
	test_t.dou_c = 3.1415926;

	for (i = 0; i < 4; i++) {
		printf ("elem_length[%d] = %d\n", i, elem_length[i]);
	}
	for (i=0, p=(int*)&(test_t); i < 4 && p != NULL; i++, p += elem_length[i]) {
		printf ("p = %p\n", p);
		printf ("test_t.%s = %d\n", elem_string[i], *p);
	}

	return 0;
}
