/***************************************************************************
* Description: 
*
**********************************History***********************************
*     when              who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  2017/06/13
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/un.h>

#include "jrd_voice_connect.h"


/*===========================================================================
  Function:  jrd_voice_connect_send_msg
===========================================================================*/
/*!
@brief
  None.

@parameters
  None.

@return
  None.
*/
/*=========================================================================*/
int jrd_voice_connect_send_msg(e_jrd_voice_socket_server_type_t server_type, jrd_voice_socket_msg_t *msg)
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
    if (E_JRD_VOICE_SOCKET_SERVER_TYPE_APLAY == server_type) {
        strcpy(server_addr.sun_path, JRD_VOICE_SOCKET_SERVER_APLAY);
    } else {
        strcpy(server_addr.sun_path, JRD_VOICE_SOCKET_SERVER_AREC);
    }

    rc = connect(connect_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (-1 == rc) {
        printf("Error, Connect AF_UNIX socket failed.\n");
        close(connect_fd);
        return -1;
    }

    /* Send the msg to server. */
    write(connect_fd, (char *)msg, sizeof(jrd_voice_socket_msg_t));
    
    close(connect_fd);
    return 0;
}
