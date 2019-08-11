/***************************************************************************
* Description: 
*
**********************************History***********************************
*     when              who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  2017/06/13
*
****************************************************************************/

#ifndef _JRD_VOICE_CONNECT_H_
#define _JRD_VOICE_CONNECT_H_

#include "jan_voice_socket.h"


/*===========================================================================
  Function:  jan_voice_connect_send_msg
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
extern int jan_voice_connect_send_msg(e_jan_voice_socket_server_type_t server_type, jan_voice_socket_msg_t *msg);


#endif /* _JRD_VOICE_CONNECT_H_ */

