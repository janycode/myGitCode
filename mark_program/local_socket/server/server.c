#include <stdio.h>
#include "../local_socket.h"

/* Like aplay / arec process. */
int main (int argc, char **argv)
{
    if (argc < 2) {
        printf ("Input error, eg: ./server 1\n");
        return -1;
    }
    server_run(argv[1]);
    return 0;
}
