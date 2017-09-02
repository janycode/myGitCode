/***************************************************************************
*     Copyright (c) 2016 by Fan.Xi.  All Rights Reserved.
****************************************************************************
* File:  jrd_voice.h
* Author:  Fan Xi <xifan2012@gmail.com>
* Description: 
*
**********************************History***********************************
*     when              who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  2017/02/23
*
****************************************************************************/

#ifndef _JRD_VOICE_H_
#define _JRD_VOICE_H_


typedef enum {
    E_JRD_VOICE_IND_INVALID = -1,
    E_JRD_VOICE_IND_SELF_CALL_STATUS_CHANGE,    /* Indication self. */
    E_JRD_VOICE_IND_CALL_STATUS_CHANGE,
    E_JRD_VOICE_IND_TIMEZONE_STATUS_CHANGE,    // add 2017.06.23
    E_JRD_VOICE_IND_MAX,
} e_jrd_voice_ind_t;

typedef enum {
    E_JRD_VOICE_ACT_ID_GET_CALL_RECORD_LIST = 0,
    E_JRD_VOICE_ACT_ID_DELETE_CALL_RECORD,
    E_JRD_VOICE_ACT_ID_CLEAR_CALL_RECORD_LIST,
    E_JRD_VOICE_ACT_ID_GET_CALL_RECORD_COUNT_INFO,
    E_JRD_VOICE_ACT_ID_MAX,
} e_jrd_voice_act_id_t;

typedef enum {
    E_JRD_VOICE_PARAM_LIST_TYPE = 0,
    E_JRD_VOICE_PARAM_TOTAL_PAGE_COUNT,
    E_JRD_VOICE_PARAM_PAGE,
    E_JRD_VOICE_PARAM_RECORD_LIST,
    E_JRD_VOICE_PARAM_RECORD_ID,
    E_JRD_VOICE_PARAM_TOTAL_COUNT_MAX,  /*5*/
    E_JRD_VOICE_PARAM_INCOMING_COUNT,
    E_JRD_VOICE_PARAM_OUTGOING_COUNT,
    E_JRD_VOICE_PARAM_MISSED_COUNT,
    E_JRD_VOICE_PARAM_TOTAL_COUNT,
} e_jrd_voice_param_t;

typedef enum {
    E_JRD_VOICE_IND_CALL_IDLE,
    E_JRD_VOICE_IND_CALL_RING,
    E_JRD_VOICE_IND_CALL_CONVERSATION,
    E_JRD_VOICE_IND_CALL_MAX,
} e_jrd_voice_call_status_t;

typedef enum
{
    E_JRD_VOICE_CONFIG_ID_INVALID = -1,
    E_JRD_VOICE_CONFIG_ID_EMERGENCY_NUMBER,
    E_JRD_VOICE_CONFIG_ID_MAX,
} e_jrd_voice_config_id_t;


/*===========================================================================
  Function:  jrd_voice_get_emergency_number
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

extern int jrd_voice_get_emergency_number (char *number_string);


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* _JRD_VOICE_H_ */
