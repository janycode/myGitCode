#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

typedef enum {
	SERVER_TYPE_APLAY = 0,
	SERVER_TYPE_AREC,
} e_socket_server_type_t;

typedef enum {
    E_SOCKET_OPT_GET,
    E_SOCKET_OPT_SET,
} e_socket_opt_t;

typedef struct socket_msg_struct {
    e_socket_opt_t sock_opt;
    char device[32];
} socket_msg_t;

int server_listen_fd(e_socket_server_type_t server_type);
int server_receive_msg(int listen_fd, socket_msg_t *msg);
int server_run(char *arg);

#endif /* _SOCKET_H_ */
