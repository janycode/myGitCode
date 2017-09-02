/***************************************************************************
* Description: 
*
**********************************History***********************************
*     when              who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  2017/02/23
*
****************************************************************************/

#ifndef _JRD_VOICE_CORE_H_
#define _JRD_VOICE_CORE_H_

#include <signal.h>
#include <pthread.h>

#include "jrd_voice.h"
#include "jrd_voice_al.h"
#include "jrd_voice_soundcard.h"
#include "jrd_voice_connect.h"

/*---------------------------------------------------------------------------*/

typedef enum {
    E_JRD_VOICE_CORE_CALL_TYPE_UNKNOWN = 0,
    E_JRD_VOICE_CORE_CALL_TYPE_INCOMING = 1,
    E_JRD_VOICE_CORE_CALL_TYPE_OUTGOING = 2,
    E_JRD_VOICE_CORE_CALL_TYPE_MISSED = 3,
} e_jrd_voice_core_call_type_t;

typedef enum {
    E_JRD_VOICE_CORE_STATUS_IDLE = 0,
    E_JRD_VOICE_CORE_STATUS_RING,
    E_JRD_VOICE_CORE_STATUS_DIALING,
    E_JRD_VOICE_CORE_STATUS_WAIT,
    E_JRD_VOICE_CORE_STATUS_ALERTING,
    E_JRD_VOICE_CORE_STATUS_CONVERSATION,
    E_JRD_VOICE_CORE_STATUS_MAX,
} e_jrd_voice_core_status_t;

typedef enum {
    E_JRD_VOICE_CORE_MSG_INVALID = -1,
    E_JRD_VOICE_CORE_MSG_FROM_NETWORK,
    E_JRD_VOICE_CORE_MSG_FROM_DEVICE,
    E_JRD_VOICE_CORE_MSG_MAX,
} e_jrd_voice_core_msg_t;

typedef enum {
    E_JRD_VOICE_LIST_TYPE_ALL = 0,
    E_JRD_VOICE_LIST_TYPE_INCOMING,
    E_JRD_VOICE_LIST_TYPE_OUTGOING,
    E_JRD_VOICE_LIST_TYPE_MISSED,
} e_jrd_voice_list_type_t;

typedef enum {
    E_JRD_VOICE_DIAL_TYPE_INVALID = -1,
    E_JRD_VOICE_DIAL_TYPE_EMERGENCY = 0,
    E_JRD_VOICE_DIAL_TYPE_NORMAL = 1,
} e_jrd_voice_dial_type_t;

/*---------------------------------------------------------------------------*/

typedef struct jrd_voice_call_record_count_info_struct {
    int max_count;
    int incoming_count;
    int outgoing_count;
    int missed_count;
    int total_count;
} jrd_voice_call_record_count_info_t;

typedef struct jrd_voice_call_record_struct {
    unsigned int id;
    char *contacts;
    char contact_number[32];

    time_t call_date;
    int call_duration;
    e_jrd_voice_core_call_type_t call_type;
} jrd_voice_call_record_t;

typedef struct jrd_voice_node_list_struct {
    ordered_list_link_type link;
    void *data;
} jrd_voice_node_list_t;

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
  Function:  jrd_voice_core_msg_handle
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
extern int jrd_voice_core_msg_handle(e_jrd_voice_core_msg_t msg_type, jrd_voice_call_info_t *call_info);


/*===========================================================================
  Function:  jrd_voice_core_get_record_list
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
ordered_list_type* jrd_voice_core_get_record_list(e_jrd_voice_list_type_t page_type);


/*===========================================================================
  Function:  jrd_voice_core_delete_record_id
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
extern int jrd_voice_core_delete_record_id(int list_type, int record_id);


/*===========================================================================
  Function:  jrd_voice_core_clear_record_list
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
extern int jrd_voice_core_clear_record_list(ordered_list_type *record_list);


/*===========================================================================
  Function:  jrd_voice_core_delete_record_from_list
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
static int jrd_voice_core_delete_record_from_list(ordered_list_type *list, int id);


/*===========================================================================
  Function:  jrd_voice_core_get_record_count_info
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
jrd_voice_call_record_count_info_t* jrd_voice_core_get_record_count_info(void);


/*===========================================================================
  Function:  jrd_voice_core_opt_ring
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
extern void jrd_voice_core_opt_ring(int opt);


/*===========================================================================
  Function:  jrd_voice_core_event_register
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
extern int jrd_voice_core_event_register(void *callback, void *cb_param);


/*===========================================================================
  Function:  jrd_voice_core_event_unregister
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
extern int jrd_voice_core_event_unregister(void *callback);


/*===========================================================================
  Function:  jrd_voice_core_init
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
extern int jrd_voice_core_init(void);

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif /* _JRD_VOICE_H_ */
