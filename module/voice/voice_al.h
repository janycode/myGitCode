/***************************************************************************
*     Copyright (c) 2016 by Fan.Xi.  All Rights Reserved.
****************************************************************************
* File:  jrd_voice_al.h
* Author:  Fan Xi <xifan2012@gmail.com>
* Description: 
*
**********************************History***********************************
*     when              who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  2017/02/28
*
****************************************************************************/

#ifndef _JRD_VOICE_AL_H_
#define _JRD_VOICE_AL_H_

#include "utils/list.h"

/*---------------------------------------------------------------------------*/

typedef enum {
    E_JRD_VOICE_CALL_STATE_INVALID = -1,
    E_JRD_VOICE_CALL_STATE_ORIGINATING,     /* 0 Origination */
    E_JRD_VOICE_CALL_STATE_INCOMING,        /* 1 Incoming */
    E_JRD_VOICE_CALL_STATE_CONVERSATION,    /* 2 Conversation */
    E_JRD_VOICE_CALL_STATE_CC_IN_PROGRESS,  /* 3 Call is originating but waiting for call control to complete */
    E_JRD_VOICE_CALL_STATE_ALERTING,        /* 4 Alerting */
    E_JRD_VOICE_CALL_STATE_HOLD,            /* 5 Hold */
    E_JRD_VOICE_CALL_STATE_WAITING,         /* 6 Waiting */
    E_JRD_VOICE_CALL_STATE_DISCONNECTING,   /* 7 Disconnecting */
    E_JRD_VOICE_CALL_STATE_END,             /* 8 End */
    E_JRD_VOICE_CALL_STATE_SETUP,           /* 9 MT call is in Setup state in 3GPP */
    E_JRD_VOICE_CALL_STATE_MAX,
} e_jrd_voice_call_state_t;

typedef enum {
    E_JRD_VOICE_CALL_TYPE_INVALID = -1,
    E_JRD_VOICE_CALL_TYPE_VOICE,          /* 0 - Voice */
    E_JRD_VOICE_CALL_TYPE_VOICE_IP,       /* 1 - Voice over IP */
    E_JRD_VOICE_CALL_TYPE_EMERGENCY,      /* 2 - Emergency */
    E_JRD_VOICE_CALL_TYPE_EMERGENCY_IP,   /* 3 - Emergency VoIP */
    E_JRD_VOICE_CALL_TYPE_MAX,
} e_jrd_voice_call_type_t;

typedef enum {
    E_JRD_VOICE_CALL_DIRECTION_INVALID = -1,
    E_JRD_VOICE_CALL_DIRECTION_MO,  /* MO call */
    E_JRD_VOICE_CALL_DIRECTION_MT,  /* MT call */
    E_JRD_VOICE_CALL_DIRECTION_MAX,
} e_jrd_voice_call_direction_t;

typedef enum {
    E_JRD_VOICE_CALL_MODE_INVALID = -1,
    E_JRD_VOICE_CALL_MODE_NO_SRV,   /* 0 - No service */
    E_JRD_VOICE_CALL_MODE_CDMA,     /* 1 - CDMA */
    E_JRD_VOICE_CALL_MODE_GSM,      /* 2 - GSM */
    E_JRD_VOICE_CALL_MODE_UMTS,     /* 3 - UMTS */
    E_JRD_VOICE_CALL_MODE_LTE,      /* 4 - LTE */
    E_JRD_VOICE_CALL_MODE_TDS,      /* 5 - TD-SCDMA */
    E_JRD_VOICE_CALL_MODE_UNKNOWN,  /* 6 - Unknown */
    E_JRD_VOICE_CALL_MODE_WLAN,     /* 7 - WLAN */
    E_JRD_VOICE_CALL_MODE_MAX,
} e_jrd_voice_call_mode_t;

typedef enum {
    E_JRD_VOICE_EVENT_INVALID = -1,
    E_JRD_VOICE_EVENT_UNKNOWN,
    E_JRD_VOICE_EVENT_CALL_STATUS_CHANGE,
    E_JRD_VOICE_EVENT_MAX,
} e_jrd_voice_event_t;

typedef enum {
    JRD_VOICE_CORE_STATUS_IDLE = 0,
    JRD_VOICE_CORE_STATUS_RING,
    JRD_VOICE_CORE_STATUS_DIALING,
    JRD_VOICE_CORE_STATUS_WAIT,
    JRD_VOICE_CORE_STATUS_ALERTING,
    JRD_VOICE_CORE_STATUS_CONVERSATION,
    JRD_VOICE_CORE_STATUS_MAX,
} jrd_voice_core_status_t;

/*---------------------------------------------------------------------------*/

typedef int (*jrd_voice_event_cb)(e_jrd_voice_call_state_t event, void *msg_data, int msg_data_len, void *cb_param);
typedef struct jrd_voice_msg_element_struct {
    jrd_voice_event_cb callback;
    void *cb_param;
} jrd_voice_event_element_t;

typedef struct jrd_voice_event_list_node_struct {
    list_link_type *link;
    jrd_voice_event_element_t event_element;
} jrd_voice_event_list_node_t;

typedef struct jrd_voice_call_info_struct {
    unsigned char call_id;
    e_jrd_voice_call_state_t call_state;
    e_jrd_voice_call_type_t call_type;
    e_jrd_voice_call_direction_t call_direction;
    e_jrd_voice_call_mode_t call_mode;
    char call_number[84];
} jrd_voice_call_info_t;

typedef struct jrd_voice_dtmf_queue_struct {
    char dtmf_num[64];
    int head;
    int tail;
} jrd_voice_dtmf_queue_t;

typedef struct jrd_voice_timezone_data_struct {   // add 2017.06.23
    char time_zone_name[64];
    char time_zone[64];
    char time_diff[64];
} jrd_voice_timezone_data_t;

os_signal_type jrd_key_signal;
os_signal_type jrd_net_signal;
jrd_voice_dtmf_queue_t jrd_dtmf_queue;

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
  Function:  jrd_voice_al_get_call_id
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
extern unsigned char jrd_voice_al_get_call_id(void);


/*===========================================================================
  Function:  jrd_voice_al_get_call_state
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
extern int jrd_voice_al_get_call_state(void);


/*===========================================================================
  Function:  jrd_voice_al_get_call_number
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
extern int jrd_voice_al_get_call_number(char *number);


/*===========================================================================
  Function:  jrd_voice_al_get_third_call_id
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
extern unsigned char jrd_voice_al_get_third_call_id(void);


/*===========================================================================
  Function:  jrd_voice_al_get_third_call_state
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
extern int jrd_voice_al_get_third_call_state(void);


/*===========================================================================
  Function:  jrd_voice_al_get_third_call_number
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
extern int jrd_voice_al_get_third_call_number(char *number);


/*===========================================================================
  Function:  jrd_voice_al_clear_third_call_state
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
extern int jrd_voice_al_clear_third_call_state(void);


/*===========================================================================
  Function:  jrd_voice_al_manage_calls
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
extern int jrd_voice_al_manage_calls(void);


/*===========================================================================
  Function:  jrd_voice_al_call_answer
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
extern int jrd_voice_al_call_answer(void);


/*===========================================================================
  Function:  jrd_voice_al_call_answer_busy
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
extern int jrd_voice_al_call_answer_busy(void);


/*===========================================================================
  Function:  jrd_voice_al_digit_dtmf_thread
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
extern void *jrd_voice_al_digit_dtmf_thread (void *data);


/*===========================================================================
  Function:  jrd_voice_al_call_dial
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
extern int jrd_voice_al_call_dial(char *call_number, int dial_type);


/*===========================================================================
  Function:  jrd_voice_al_call_end
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
extern int jrd_voice_al_call_end(void);


/*===========================================================================
  Function:  jrd_voice_al_event_register
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
extern int jrd_voice_al_event_register(void *callback, void *cb_param);


/*===========================================================================
  Function:  jrd_voice_al_event_unregister
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
extern int jrd_voice_al_event_unregister(void *callback);


/*===========================================================================
  Function:  jrd_voice_al_init
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
extern int jrd_voice_al_init(void);


/*===========================================================================
  Function:  jrd_voice_al_exit
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
extern int jrd_voice_al_exit(void);


#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif /* _JRD_VOICE_AL_H_ */
