#include "local_socket.h"

#define SOCKET_SERVER_APLAY_FILE  "../aplay_server"
#define SOCKET_SERVER_AREC_FILE  "../arec_server"

int server_listen_fd(e_socket_server_type_t server_type)
{
    struct sockaddr_un server_addr;
    int listen_fd;
    int rc = 0;

    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        printf("Error, Socket AF_UNIX socket failed.\n");
        return -1;
    }

    server_addr.sun_family = AF_UNIX;
    printf("Info, server_type = %d.\n", server_type);
    if (SERVER_TYPE_APLAY == server_type) {
        strcpy(server_addr.sun_path, SOCKET_SERVER_APLAY_FILE);  // /var/aplay_server
    } else {
        strcpy(server_addr.sun_path, SOCKET_SERVER_AREC_FILE);   // /var/arec_server
    }

    unlink(server_addr.sun_path);
    /* bind */
    rc = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (-1 == rc) {
        printf("Error, Bind server socket failed.\n");
        close(listen_fd);
        unlink(server_addr.sun_path);
        return -1;
    }

    rc = listen(listen_fd, 1);
    if (-1 == rc) {
        printf("Error, Listen client connect failed.\n");
        close(listen_fd);
        unlink(server_addr.sun_path);
        return -1;
    }

    chmod(server_addr.sun_path, 00777);
    printf("Info, listen_fd = %d.\n", listen_fd);
    return listen_fd;

}

int socket_send_msg(e_socket_server_type_t server_type, socket_msg_t *msg)
{
    struct sockaddr_un server_addr;
    int connect_fd;
    int rc = -1;

    connect_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect_fd < 0) {
        printf("Error, Socket AF_UNIX socket failed.\n");
        return -1;
    }

    server_addr.sun_family = AF_UNIX;
    if (SERVER_TYPE_APLAY == server_type) {
        strcpy(server_addr.sun_path, SOCKET_SERVER_APLAY_FILE);
    } else {
        strcpy(server_addr.sun_path, SOCKET_SERVER_AREC_FILE);
    }

    rc = connect(connect_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (-1 == rc) {
        printf("Error, Connect AF_UNIX socket failed.\n");
        close(connect_fd);
        return -1;
    }

    /* Send the msg to server. */
    write(connect_fd, (char *)msg, sizeof(socket_msg_t));
    close(connect_fd);
    return 0;
}


int server_receive_msg(int listen_fd, socket_msg_t *msg)
{
    fd_set fdsr;
    struct sockaddr_un client_addr;
    socket_msg_t tmp_msg = {0};
    int accept_fd = -1;
    int len = 0;
    int rc = -1;

    FD_ZERO(&fdsr);
    FD_SET(listen_fd, &fdsr);

    /* Select */
    rc = select((listen_fd + 1), &fdsr, NULL, NULL, NULL);   /* @timeout pargam set NULL to block. */
    if (rc < 0) {
        printf("Error, Select AF_UNIX socket failed.\n");
        return -1;
    } else if (0 == rc) {
        return 0;
    } else {
        /* Nothing to do. */
    }

    /* Accept */
    len = sizeof(client_addr);
    accept_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &len);
    if (accept_fd < 0) {
        printf("Error, Accept server socket failed.\n");
        return -1;
    }

    memset(&tmp_msg, 0, sizeof(tmp_msg));
    read(accept_fd, (char *)&tmp_msg, sizeof(tmp_msg));

    switch(tmp_msg.sock_opt)
    {
        case E_SOCKET_OPT_GET:
        {
            printf("Info, None information to get.\n");
            break;
        }
        case E_SOCKET_OPT_SET:
        {
            memset(msg, 0, sizeof(socket_msg_t));
            memcpy(msg, &tmp_msg, sizeof(socket_msg_t));
            break;
        }
        default :
        {
            printf("Error, Socket operation is %d.\n", tmp_msg.sock_opt);
            break;
        }
    }
    printf ("Received msg: %s\n", msg->device);

    close(accept_fd);
    return 1;
}



int server_run(char *arg)
{
    int aplay_fd = -1;
    socket_msg_t msg = {0};
    char *device = "param";
    e_socket_server_type_t run_type = -1;

    strncpy(msg.device, device, strlen(device));
    run_type = atoi(arg);

    aplay_fd = server_listen_fd(run_type);
    if (-1 == aplay_fd) {
        fprintf(stderr, "Aplay: Get server socket fd, failed Error no %d \n", errno);
        return -errno;
    }

    printf ("source msg: %s\n", device);
    while (1) {
        if (0 >= server_receive_msg(aplay_fd, &msg)) {
            fprintf(stderr, "Aplay: Receive msg failed, device: %s; Error no %d \n", msg.device, errno);
        }
    }
}
