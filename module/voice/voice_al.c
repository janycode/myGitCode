/***************************************************************************
* Description: 
*
**********************************History***********************************
*     when              who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  2017/02/28
*
****************************************************************************/

#include "jan_oem.h"
#include "jan_oem_common.h"

#include "jan_qmi_common.h"
#include "jan_qmi_voice.h"
#include "jan_voice_al.h"

static list_type jan_voice_event_list = {0};
static jan_voice_call_info_t g_call_info = {0};
static jan_voice_call_info_t g_third_call_info = {0};

static pthread_t jan_dtmf_thrd;

#define JRD_VOICE_DEBUG

/*===========================================================================
  Function:  jan_voice_al_decode_call_info
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
static int jan_voice_al_decode_call_info(voice_call_info2_type_v02 *src_data, jan_voice_call_info_t *decode_data)
{
    e_jan_voice_call_state_t call_state = E_JRD_VOICE_CALL_STATE_INVALID;
    e_jan_voice_call_type_t call_type = E_JRD_VOICE_CALL_TYPE_INVALID;
    e_jan_voice_call_direction_t call_direction = E_JRD_VOICE_CALL_DIRECTION_INVALID;
    e_jan_voice_call_mode_t call_mode = E_JRD_VOICE_CALL_MODE_INVALID;
    int call_id = 0;

    if (!src_data || !decode_data) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, param pointer is NULL.\n");
        return JRD_FAIL;
    }

/* Make sure the events are for the same call ID that is set in global. */
/*
    if((src_data->call_id == g_call_info.call_id) || (255 == g_call_info.call_id)) {
        if ((src_data->call_id != g_call_info.call_id)
            && (255 == g_call_info.call_id) 
            && (src_data->call_id != 0)) {
*/
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, call_state==%d. call_id=%d.. here.\n", src_data->call_state, src_data->call_id); 
    if (src_data->call_id != 0) {
            call_id = src_data->call_id;
    }
        
    switch (src_data->call_state)
    {
        case CALL_STATE_ORIGINATING_V02:  /* 0x01 - Origination */
        {
            call_state = E_JRD_VOICE_CALL_STATE_ORIGINATING;
            break;
        }
        case CALL_STATE_INCOMING_V02:  /* 0x02 - Incoming */
        {
            call_state = E_JRD_VOICE_CALL_STATE_INCOMING;
            break;
        }
        case CALL_STATE_CONVERSATION_V02:  /* 0x03 - Conversation */
        {
            call_state = E_JRD_VOICE_CALL_STATE_CONVERSATION;
            break;
        }
        case CALL_STATE_CC_IN_PROGRESS_V02:  /* 0x04 - Call is originating but waiting for call control to complete */
        {
            call_state = E_JRD_VOICE_CALL_STATE_CC_IN_PROGRESS;
            break;
        }
        case CALL_STATE_ALERTING_V02:  /* 0x05 - Alerting */
        {
            call_state = E_JRD_VOICE_CALL_STATE_ALERTING;
            break;
        }
        case CALL_STATE_HOLD_V02:  /* 0x06 - Hold */
        {
            call_state = E_JRD_VOICE_CALL_STATE_HOLD;
            break;
        }
        case CALL_STATE_WAITING_V02:  /* 0x07 - Waiting */
        {
            call_state = E_JRD_VOICE_CALL_STATE_WAITING;
            break;
        }
        case CALL_STATE_DISCONNECTING_V02:  /* 0x08 - Disconnecting */
        {
            call_state = E_JRD_VOICE_CALL_STATE_DISCONNECTING;
            break;
        }
        case CALL_STATE_END_V02:  /* 0x09 - End */
        {
            call_state = E_JRD_VOICE_CALL_STATE_END;
            break;
        }
        case CALL_STATE_SETUP_V02:  /* 0x0A - MT call is in Setup state in 3GPP */
        {
            call_state = E_JRD_VOICE_CALL_STATE_SETUP;
            break;
        }
        default:
        {
            call_state = E_JRD_VOICE_CALL_STATE_INVALID;
            break;
        }
    }

    switch (src_data->call_type)
    {
        case CALL_TYPE_VOICE_V02:  /* 0x00 - Voice */
        {
            call_type = E_JRD_VOICE_CALL_TYPE_VOICE;
            break;
        }
        case CALL_TYPE_VOICE_IP_V02:  /* 0x02 - Voice over IP */
        {
            call_type = E_JRD_VOICE_CALL_TYPE_VOICE_IP;
            break;
        }
        case CALL_TYPE_EMERGENCY_V02:  /* 0x09 - Emergency */
        {
            call_type = E_JRD_VOICE_CALL_TYPE_EMERGENCY;
            break;
        }
        case CALL_TYPE_EMERGENCY_IP_V02:  /* 0x0B - Emergency VoIP */
        {
            call_type = E_JRD_VOICE_CALL_TYPE_EMERGENCY_IP;
            break;
        }
        default:
        {
            call_type = E_JRD_VOICE_CALL_TYPE_INVALID;
            break;
        }
    }

    switch (src_data->direction)
    {
        case CALL_DIRECTION_MO_V02:  /* 0x01 - MO call */
        {
            call_direction = E_JRD_VOICE_CALL_DIRECTION_MO;
            break;
        }
        case CALL_DIRECTION_MT_V02:  /* 0x02 - MT call */
        {
            call_direction = E_JRD_VOICE_CALL_DIRECTION_MT;
            break;
        }
        default:
        {
            call_direction = E_JRD_VOICE_CALL_DIRECTION_INVALID;
            break;
        }
    }

    switch (src_data->mode)
    {
        case CALL_MODE_NO_SRV_V02:  /* 0x00 - No service */
        {
            call_mode = E_JRD_VOICE_CALL_MODE_NO_SRV;
            break;
        }
        case CALL_MODE_CDMA_V02:  /* 0x01 - CDMA */
        {
            call_mode = E_JRD_VOICE_CALL_MODE_CDMA;
            break;
        }
        case CALL_MODE_GSM_V02:  /* 0x02 - GSM */
        {
            call_mode = E_JRD_VOICE_CALL_MODE_GSM;
            break;
        }
        case CALL_MODE_UMTS_V02:  /* 0x03 - UMTS */
        {
            call_mode = E_JRD_VOICE_CALL_MODE_UMTS;
            break;
        }
        case CALL_MODE_LTE_V02:  /* 0x04 - LTE */
        {
            call_mode = E_JRD_VOICE_CALL_MODE_LTE;
            break;
        }
        case CALL_MODE_TDS_V02:  /* 0x05 - TD-SCDMA */
        {
            call_mode = E_JRD_VOICE_CALL_MODE_TDS;
            break;
        }
        case CALL_MODE_UNKNOWN_V02:  /* 0x06 - Unknown */
        {
            call_mode = E_JRD_VOICE_CALL_MODE_UNKNOWN;
            break;
        }
        case CALL_MODE_WLAN_V02:  /* 0x07 - WLAN */
        {
            call_mode = E_JRD_VOICE_CALL_MODE_WLAN;
            break;
        }
        default:
        {
            call_mode = E_JRD_VOICE_CALL_MODE_INVALID;
            break;
        }
    }

    decode_data->call_state = call_state;
    decode_data->call_type = call_type;
    decode_data->call_direction = call_direction;
    decode_data->call_mode = call_mode;
    decode_data->call_id = call_id;

    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, call_state = %d, call_id = %d.\n", decode_data->call_state, decode_data->call_id);
    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_al_msg_cb
===========================================================================*/
/*!
@brief
  The call back function which register to voice qmi client, when voice qmi has
notify event process it.

@parameters
  None.

@return
  None.
*/
/*=========================================================================*/
static void jan_voice_al_msg_cb(
  unsigned int msg_id,
  void *msg_data,
  unsigned int msg_data_len)
{
    list_link_type *list_node_link = NULL;
    jan_voice_event_list_node_t *this_msg_node = NULL;
    e_jan_voice_event_t event = E_JRD_VOICE_EVENT_INVALID;
    void *event_data = NULL;
    unsigned int event_data_len = 0;
    int rc = JRD_NO_ERR;

    switch (msg_id)
    {
        case QMI_VOICE_DTMF_IND_V02:
        {
            voice_dtmf_ind_msg_v02 *dtmf_msg_data = (voice_dtmf_ind_msg_v02 *)msg_data;

            #ifdef JRD_VOICE_DEBUG
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_id: %d.\n", dtmf_msg_data->dtmf_info.call_id);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, dtmf_event: 0x%x.\n", dtmf_msg_data->dtmf_info.dtmf_event);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, digit_buffer_len: %d.\n", dtmf_msg_data->dtmf_info.digit_buffer_len);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, digit_buffer: %s.\n", dtmf_msg_data->dtmf_info.digit_buffer);
            #endif
#if 1
            /* DTMF_EVENT_REV_START_CONT == 0x01 */
            if (dtmf_msg_data->dtmf_info.dtmf_event == 0x01)
            {
                pthread_cond_signal (&jan_net_signal.cond);
                #ifdef JRD_VOICE_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [dtmf_event] START signal was sent.\n");
                #endif
            }

            /* DTMF_EVENT_REV_STOP_CONT == 0x03 */
            if (dtmf_msg_data->dtmf_info.dtmf_event == 0x03)
            {
                pthread_cond_signal (&jan_net_signal.cond);
                #ifdef JRD_VOICE_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [dtmf_event] STOP signal was sent.\n");
                #endif
            }
#endif
            break;
        }
        case QMI_VOICE_ALL_CALL_STATUS_IND_V02:
        {
            event = E_JRD_VOICE_EVENT_CALL_STATUS_CHANGE;

            voice_all_call_status_ind_msg_v02 *voice_call_status_msg_data = (voice_all_call_status_ind_msg_v02 *)msg_data;
            jan_voice_call_info_t call_info[QMI_VOICE_CALL_INFO_MAX_V02] = {0};
            int index = 0;

            JRD_MALLOC(sizeof(jan_voice_call_info_t), event_data);
            if (!event_data) {
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, malloc voice call info data.\n");
                return;
            }

            for (index = 0; index < QMI_VOICE_CALL_INFO_MAX_V02; index++) {
                rc = jan_voice_al_decode_call_info(&(voice_call_status_msg_data->call_info[index]), &call_info[index]);
                if (rc != JRD_NO_ERR) {
                    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, decode card status change ind message.\n");
                    return;
                }
                if (voice_call_status_msg_data->remote_party_number_valid) {
                    JRD_STRNCPY(call_info[index].call_number, voice_call_status_msg_data->remote_party_number[index].number, sizeof(voice_call_status_msg_data->remote_party_number[index].number));
                }
                event_data_len = sizeof(jan_voice_call_info_t);

                if (call_info[index].call_id == 0)
                    break;

                #ifdef JRD_VOICE_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, QMI call_info[%d].call_number  =%s.\n", index, call_info[index].call_number);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, QMI call_info[%d].call_id      =%d.\n", index, call_info[index].call_id);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, QMI call_info[%d].call_state   =%d.\n", index, call_info[index].call_state);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, QMI call_info[%d].call_type    =%d.\n", index, call_info[index].call_type);
                #endif
            }

            if (index == 2) {
                /* Event_data or g_third_call_info is 3rd call info. */
                JRD_MEMCPY(event_data, &call_info[0], event_data_len);
                JRD_MEMCPY(&g_third_call_info, &call_info[0], sizeof(jan_voice_call_info_t));
                JRD_MEMCPY(&g_call_info, &call_info[1], sizeof(jan_voice_call_info_t));

                #ifdef JRD_VOICE_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, 3rd call_number  =%s.\n", g_third_call_info.call_number);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, 3rd call_state   =%d.\n", g_third_call_info.call_state);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, 2nd call_number  =%s.\n", g_call_info.call_number);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, 2nd call_state   =%d.\n", g_call_info.call_state);
                #endif

            } else {
                JRD_MEMCPY(event_data, &call_info[0], event_data_len);
                JRD_MEMCPY(&g_call_info, &call_info[0], sizeof(jan_voice_call_info_t));

                #ifdef JRD_VOICE_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, current call_number  =%s.\n", g_call_info.call_number);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, current call_state   =%d.\n", g_call_info.call_state);
                #endif
            }
            break;
        }
        default:
        {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW,"Info, unknown the voice indication callback msg_id: %d.\n", msg_id);
            return;
        }
    }

    /* To callback function list to module. */
    if (list_size(&jan_voice_event_list) > 0) {
        list_node_link = list_peek_front(&jan_voice_event_list);
    }

    while (list_node_link != NULL) {
        this_msg_node = (jan_voice_event_list_node_t *)list_node_link;

        if (this_msg_node->event_element.callback) {
            this_msg_node->event_element.callback(event, event_data, event_data_len, this_msg_node->event_element.cb_param);
        }
        list_node_link =  (list_link_type *) list_peek_next(&jan_voice_event_list, list_node_link);
    }/* end while (this_msg_node...) */

    if (event_data) {
        JRD_FREE(event_data);
        event_data = NULL;
    }
    return;
}


/*===========================================================================
  Function:  jan_voice_al_get_call_id
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
unsigned char jan_voice_al_get_call_id(void)
{
    return g_call_info.call_id;
}


/*===========================================================================
  Function:  jan_voice_al_get_call_state
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
int jan_voice_al_get_call_state(void)
{
    return g_call_info.call_state;
}


/*===========================================================================
  Function:  jan_voice_al_get_call_number
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
int jan_voice_al_get_call_number(char *number)
{
    JRD_STRNCPY(number, g_call_info.call_number, JRD_STRLEN(g_call_info.call_number));
    return 0;
}


/*===========================================================================
  Function:  jan_voice_al_get_third_call_id
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
unsigned char jan_voice_al_get_third_call_id(void)
{
    return g_third_call_info.call_id;
}


/*===========================================================================
  Function:  jan_voice_al_get_third_call_state
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
int jan_voice_al_get_third_call_state(void)
{
    return g_third_call_info.call_state;
}


/*===========================================================================
  Function:  jan_voice_al_get_third_call_number
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
int jan_voice_al_get_third_call_number(char *number)
{
    JRD_STRNCPY(number, g_third_call_info.call_number, JRD_STRLEN(g_third_call_info.call_number));
    return 0;
}


/*===========================================================================
  Function:  jan_voice_al_clear_third_call_state
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
int jan_voice_al_clear_third_call_state(void)
{
    g_third_call_info.call_state = E_JRD_VOICE_CALL_STATE_INVALID;
    return 0;
}


/*===========================================================================
  Function:  jan_voice_al_manage_calls
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
int jan_voice_al_manage_calls(void)
{
    int rc = JRD_NO_ERR;
    sups_type_enum_v02 sups_type = SUPS_TYPE_CCBS_ACTIVATION_V02; // 0x07

    rc = jan_qmi_voice_manage_calls(g_call_info.call_id, sups_type);
    if (rc != QMI_NO_ERR) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, manage call failed.\n");
        return JRD_FAIL;
    }
    return JRD_NO_ERR;
}



/*===========================================================================
  Function:  jan_voice_al_call_answer
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
int jan_voice_al_call_answer(void)
{
    int rc = JRD_NO_ERR;

    if (jan_voice_al_get_call_state() == E_JRD_VOICE_CALL_STATE_DISCONNECTING
        || jan_voice_al_get_call_state() == E_JRD_VOICE_CALL_STATE_END)
    {
        rc = jan_qmi_voice_call_answer(g_third_call_info.call_id);
        if (rc != QMI_NO_ERR) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, answer call failed.\n");
            return JRD_FAIL;
        }
    } else
    {
        rc = jan_qmi_voice_call_answer(g_call_info.call_id);
        if (rc != QMI_NO_ERR) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, answer call failed.\n");
            return JRD_FAIL;
        }
    }
    return JRD_NO_ERR;
}

/*===========================================================================
  Function:  jan_voice_al_call_answer_busy
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
int jan_voice_al_call_answer_busy(void)
{
    int rc = JRD_NO_ERR;

    rc = jan_qmi_voice_call_answer_busy(g_call_info.call_id);
    if (rc != QMI_NO_ERR) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, answer call failed.\n");
        return JRD_FAIL;
    }
    return JRD_NO_ERR;
}

/*===========================================================================
  Function:  jan_voice_al_digit_dtmf_thread
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
#if 1
void *jan_voice_al_digit_dtmf_thread (void *data)
{
    int rc = 0;
    uint32 req_signal = 0;
    char digit_dtmf_num = 0;
    jan_voice_dtmf_queue_t *p_dtmf_queue = &jan_dtmf_queue;

    while (1)
    {
        JRD_LOCK (&jan_key_signal);
        
        /* Dtmf queue is empty, waiting for new key_signal. */
        if (p_dtmf_queue->head == p_dtmf_queue->tail)
        {
            pthread_cond_wait (&jan_key_signal.cond, &jan_key_signal.mutex);
        }
        
        while (p_dtmf_queue->head < p_dtmf_queue->tail)
        {
            digit_dtmf_num = p_dtmf_queue->dtmf_num[p_dtmf_queue->head];
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, digit_num: %d.\n", digit_dtmf_num);

            /* Start dtmf to send number. */
            rc = jan_qmi_voice_start_dtmf (g_call_info.call_id, digit_dtmf_num);
            if (rc != QMI_NO_ERR)
            {
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Error, dtmf number send failed.\n");
                return NULL;
            } else {
                //pthread_cond_wait (&jan_net_signal.cond, &jan_net_signal.mutex);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "--->>[START]<<--- cond_wait recvived signal.\n");
            }
            
            /* Stop dtmf after recv ack success. */
            rc = jan_qmi_voice_stop_dtmf (g_call_info.call_id);
            if (rc != QMI_NO_ERR)
            {
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, dtmf number stop failed.\n");
                return NULL;
            } else {
                //pthread_cond_wait (&jan_net_signal.cond, &jan_net_signal.mutex);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "--->>[STOP]<<--- cond_wait recvived signal.\n");
            }

            p_dtmf_queue->head++;
        }
        
        JRD_UNLOCK(&jan_key_signal);
    }
    return;
}
#endif

/*===========================================================================
  Function:  jan_voice_al_call_dial
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
int jan_voice_al_call_dial(char *call_number, int dial_type)
{
    int rc = JRD_NO_ERR;

    if (!call_number) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, param pointer is NULL.\n");
        return JRD_FAIL;
    }

    rc = jan_qmi_voice_call_dial(&g_call_info.call_id, call_number, JRD_STRLEN(call_number), dial_type);
    if (rc != QMI_NO_ERR) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, dial call failed.\n");
        return JRD_FAIL;
    }
    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_al_call_end
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
int jan_voice_al_call_end(void)
{
    int rc = JRD_NO_ERR;

    /* For 3rd incoming call, end the current call of conversation. */
    if (g_third_call_info.call_state == CALL_STATE_WAITING_V02)
    {
        rc = jan_qmi_voice_call_end(g_third_call_info.call_id);
        if (rc != QMI_NO_ERR) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, end call failed.\n");
            return JRD_FAIL;
        }
    } else {
        rc = jan_qmi_voice_call_end(g_call_info.call_id);
        if (rc != QMI_NO_ERR) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, end call failed.\n");
            return JRD_FAIL;
        }
    }
    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_al_event_register
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
int jan_voice_al_event_register(void *callback, void *cb_param)
{
    jan_voice_event_list_node_t *new_event_node = NULL;

    if (!callback) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, callback pointer is NULL.\n");
        return JRD_FAIL;
    }

    JRD_MALLOC(sizeof(jan_voice_event_list_node_t), new_event_node);
    if (!new_event_node) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, malloc new event node.\n");
        return JRD_FAIL;
    }

    new_event_node->event_element.callback = callback;
    new_event_node->event_element.cb_param = cb_param;

    /* All new entries will be inserted at the end of the list. */
    list_push_back(&jan_voice_event_list, (list_link_type *)new_event_node);
    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_al_event_unregister
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
int jan_voice_al_event_unregister(void *callback)
{
    jan_voice_event_list_node_t *del_event_node = NULL;

    if (!callback) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, callback pointer is NULL.\n");
        return JRD_FAIL;
    }

    del_event_node = (jan_voice_event_list_node_t *)list_peek_front(&jan_voice_event_list);
    while (del_event_node != NULL) {
        if (del_event_node->event_element.callback == callback) {
            list_pop_item(&jan_voice_event_list, (list_link_type *)del_event_node);
            JRD_FREE(del_event_node);
            del_event_node = NULL;
        } else {
            del_event_node = (jan_voice_event_list_node_t *)list_peek_next(&jan_voice_event_list, del_event_node->link);
        }
    }/* end while (del_event_node...) */
    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_al_init
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
int jan_voice_al_init(void)
{
    int rc = JRD_NO_ERR;

    /* Register voice indication msg. */
    rc = jan_qmi_voice_msg_register(jan_voice_al_msg_cb);
    if (rc != QMI_NO_ERR) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, register qmi voice msg.\n");
        return JRD_FAIL;
    }

    jan_qmi_voice_indication_register();
    g_call_info.call_id = 255;

#if 1
    JRD_OS_SIGNAL_INIT (&jan_key_signal);
    JRD_OS_SIGNAL_INIT (&jan_net_signal);
    if (pthread_create(&jan_dtmf_thrd, NULL, jan_voice_al_digit_dtmf_thread, NULL) < 0)
    {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Error, dtmf_thread create thread failed\n");
    }
#endif
    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_al_exit
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
int jan_voice_al_exit(void)
{
    /* Unregister. */
    jan_qmi_voice_msg_unregister(jan_voice_al_msg_cb);
    return JRD_NO_ERR;
}
