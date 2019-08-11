#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../local_socket.h"

int main (int argc, char **argv)
{
    socket_msg_t msg = {0};
    int i = 0;

    msg.sock_opt = E_SOCKET_OPT_SET;
    strncpy (msg.device, argv[2], strlen(argv[2]));
    for (i = 1; i < 99; i++) {
        if (i % 2 == 1) {
            sleep(1);
            socket_send_msg(atoi(argv[1]), &msg);
        } else {
            sleep(2);
            socket_send_msg(atoi(argv[1]), &msg);
        }
    }

    return 0;
}
