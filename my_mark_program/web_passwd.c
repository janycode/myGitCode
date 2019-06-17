#include <stdlib.h>
#include <stdio.h>

static char admin_dic[]=
{'3','4','6','7','9','a','b','c','d','e','f','g',
'h','i','k','m','n','p','q','r','t','u','v','w',
'x','y'};

static unsigned long long randseed;
static void jrd_rand_init_seed(char* seed)
{
    randseed = atoll(seed);
}

static unsigned long long jrd_rand(void)
{
    unsigned long long r;
    
    /*basic the imei, return a random num in unsigned long long type that 64 bits*/
    r = randseed = randseed * 9223 + 12345678;
    return (r << 32) | ((r >> 32) & 0xFFFFFFFF);
}


int jrd_get_hub_random_string_algo(char *rand_str, int rand_str_len, char *imei, char *dic, int dic_size)
{
    int i, idx;
    if(rand_str == NULL || imei == NULL || dic == NULL) return -1;
    
    /*init the rand_seed by imei*/ 
    jrd_rand_init_seed(imei);
    
    for(i=0; i<rand_str_len; i++)
    {
        idx = jrd_rand() % dic_size;
        rand_str[i] = dic[idx];
        
    /*avoid rand_str include the same words be neighbor like "aa" "33" */    
        if(i != 0 && rand_str[i-1] == rand_str[i]) 
            rand_str[i] = dic[(idx+1)%dic_size]; //make the dic in cyclic use 
    }

    /*the rand_str buff len must larger than the rand_str_len*/
    rand_str[rand_str_len] = '\0';
    return 0;
}

int main (int argc, char *argv[]) {
	char pwd[32] = {0};
	printf ("IMEI = %s\n", argv[1]);

	jrd_get_hub_random_string_algo(pwd, 8, argv[1], admin_dic, sizeof(admin_dic)/sizeof(admin_dic[0]));
	printf ("pwd = %s\n", pwd);
	return 0;
}
