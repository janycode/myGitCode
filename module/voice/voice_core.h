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

#include "jan_voice.h"
#include "jan_voice_al.h"
#include "jan_voice_soundcard.h"
#include "jan_voice_connect.h"

/*---------------------------------------------------------------------------*/

typedef enum {
    E_JRD_VOICE_CORE_CALL_TYPE_UNKNOWN = 0,
    E_JRD_VOICE_CORE_CALL_TYPE_INCOMING = 1,
    E_JRD_VOICE_CORE_CALL_TYPE_OUTGOING = 2,
    E_JRD_VOICE_CORE_CALL_TYPE_MISSED = 3,
} e_jan_voice_core_call_type_t;

typedef enum {
    E_JRD_VOICE_CORE_STATUS_IDLE = 0,
    E_JRD_VOICE_CORE_STATUS_RING,
    E_JRD_VOICE_CORE_STATUS_DIALING,
    E_JRD_VOICE_CORE_STATUS_WAIT,
    E_JRD_VOICE_CORE_STATUS_ALERTING,
    E_JRD_VOICE_CORE_STATUS_CONVERSATION,
    E_JRD_VOICE_CORE_STATUS_MAX,
} e_jan_voice_core_status_t;

typedef enum {
    E_JRD_VOICE_CORE_MSG_INVALID = -1,
    E_JRD_VOICE_CORE_MSG_FROM_NETWORK,
    E_JRD_VOICE_CORE_MSG_FROM_DEVICE,
    E_JRD_VOICE_CORE_MSG_MAX,
} e_jan_voice_core_msg_t;

typedef enum {
    E_JRD_VOICE_LIST_TYPE_ALL = 0,
    E_JRD_VOICE_LIST_TYPE_INCOMING,
    E_JRD_VOICE_LIST_TYPE_OUTGOING,
    E_JRD_VOICE_LIST_TYPE_MISSED,
} e_jan_voice_list_type_t;

typedef enum {
    E_JRD_VOICE_DIAL_TYPE_INVALID = -1,
    E_JRD_VOICE_DIAL_TYPE_EMERGENCY = 0,
    E_JRD_VOICE_DIAL_TYPE_NORMAL = 1,
} e_jan_voice_dial_type_t;

/*---------------------------------------------------------------------------*/

typedef struct jan_voice_call_record_count_info_struct {
    int max_count;
    int incoming_count;
    int outgoing_count;
    int missed_count;
    int total_count;
} jan_voice_call_record_count_info_t;

typedef struct jan_voice_call_record_struct {
    unsigned int id;
    char *contacts;
    char contact_number[32];

    time_t call_date;
    int call_duration;
    e_jan_voice_core_call_type_t call_type;
} jan_voice_call_record_t;

typedef struct jan_voice_node_list_struct {
    ordered_list_link_type link;
    void *data;
} jan_voice_node_list_t;

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
  Function:  jan_voice_core_msg_handle
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
extern int jan_voice_core_msg_handle(e_jan_voice_core_msg_t msg_type, jan_voice_call_info_t *call_info);


/*===========================================================================
  Function:  jan_voice_core_get_record_list
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
ordered_list_type* jan_voice_core_get_record_list(e_jan_voice_list_type_t page_type);


/*===========================================================================
  Function:  jan_voice_core_delete_record_id
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
extern int jan_voice_core_delete_record_id(int list_type, int record_id);


/*===========================================================================
  Function:  jan_voice_core_clear_record_list
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
extern int jan_voice_core_clear_record_list(ordered_list_type *record_list);


/*===========================================================================
  Function:  jan_voice_core_delete_record_from_list
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
static int jan_voice_core_delete_record_from_list(ordered_list_type *list, int id);


/*===========================================================================
  Function:  jan_voice_core_get_record_count_info
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
jan_voice_call_record_count_info_t* jan_voice_core_get_record_count_info(void);


/*===========================================================================
  Function:  jan_voice_core_opt_ring
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
extern void jan_voice_core_opt_ring(int opt);


/*===========================================================================
  Function:  jan_voice_core_event_register
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
extern int jan_voice_core_event_register(void *callback, void *cb_param);


/*===========================================================================
  Function:  jan_voice_core_event_unregister
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
extern int jan_voice_core_event_unregister(void *callback);


/*===========================================================================
  Function:  jan_voice_core_init
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
extern int jan_voice_core_init(void);

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif /* _JRD_VOICE_H_ */
