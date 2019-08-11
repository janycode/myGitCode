/***************************************************************************
* Description: 
*
**********************************History***********************************
*     when              who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  2017/02/23
*
****************************************************************************/

#include "jan_oem.h"
#include "jan_oem_common.h"

#include "jan_slic.h"
#include "jan_voice_core.h"


#define JRD_VOICE_SLIC_SPIDEV  "/dev/spidev1.0"
#define JRD_VOICE_RECORD_MAX_NUM            100
#define JRD_VOICE_CALL_OUT_NUMBER           32
#define JRD_VOICE_DTMF_NUMBER_LEN           64
#define JRD_VOICE_CALLOUT_TIMEOUT           30      // no answer for a long time (Protocol: 90s)
#define JRD_VOICE_CALL_WAIT_TIME            15      // wait 15s
#define JRD_VOICE_DIALING_OUT_TIME          5       // dialing 5s every key

/* Emergency number and length. */
#define JRD_VOICE_EMERGENCY_NUM             10
#define JRD_VOICE_EMERGENCY_NUM_LEN         6
#define JRD_VOICE_EMERGENCY_BUFF_LEN        64

#define JRD_VOICE_DEBUG
#define JRD_VOICE_STATUS_DEBUG

typedef struct jan_voice_data_struct {
    jan_slic_device_t *slic_dev;
    e_jan_slic_device_t dev_state;
    timer_info_t call_timer;

    e_jan_voice_core_status_t curr_status;
    e_jan_voice_core_status_t late_status;
    char call_num[JRD_VOICE_CALL_OUT_NUMBER];
    char dtmf_num;
    int len;
    int tone_flag; /* For 3rd tone play only once. */

    int is_incoming;
    int is_online;
    time_t previous_time;
    ordered_list_type total, missed, incoming, outgoing;
    jan_voice_call_record_count_info_t count_info;

    unsigned int id_mask[4];
} jan_voice_data_t;

static jan_voice_data_t g_voice_data = {0};
static long timezone_diff = 0;

#define JRD_VOICE_ID_MASK_BIT_SET(val, bit) ((val)[((bit) / 32)] & (1 << ((bit) % 32)))

#if 0
#define JRD_VOICE_ID_MASK_BIT_SET(val, bit)    \
    do {
        if ((val[(bit / 32)] & (1 << (bit % 32)))) { \

        } \
    } while(1);
#endif

#ifdef JRD_VOICE_DEBUG
/*===========================================================================
  Function:  jan_voice_core_show_node
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
static int jan_voice_core_show_node(void)
{
    char tmp_buf[32] = {0};
    ordered_list_link_type *list_node_link = NULL;
    jan_voice_call_record_t *record_ptr = NULL;

    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, total = %d.\n", ordered_list_size(&g_voice_data.total));
    if (ordered_list_size(&g_voice_data.total) > 0) {
        list_node_link = ordered_list_peek_front(&g_voice_data.total);
    }

    while (list_node_link != NULL) {
        record_ptr = (jan_voice_call_record_t *)(((jan_voice_node_list_t *)list_node_link)->data);

        if (record_ptr) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Total]contact_number = %s.\n", record_ptr->contact_number);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Total]call_duration = %d.\n", record_ptr->call_duration);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Total]call_type = %d.\n", record_ptr->call_type);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Total]id = %d.\n", record_ptr->id);
        }
        list_node_link =  (ordered_list_link_type *) ordered_list_peek_next(&g_voice_data.total, list_node_link);
    }/* end while (this_msg_node...) */
    
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, missed = %d.\n", ordered_list_size(&g_voice_data.missed));
    if (ordered_list_size(&g_voice_data.missed) > 0) {
        list_node_link = ordered_list_peek_front(&g_voice_data.missed);
    }

    while (list_node_link != NULL) {
        record_ptr = (jan_voice_call_record_t *)((jan_voice_node_list_t *)(((jan_voice_node_list_t *)list_node_link)->data))->data;

        if (record_ptr) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Missed]contact_number = %s.\n", record_ptr->contact_number);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Missed]call_duration = %d.\n", record_ptr->call_duration);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Missed]call_type = %d.\n", record_ptr->call_type);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Missed]id = %d.\n", record_ptr->id);
        }
        list_node_link =  (ordered_list_link_type *) ordered_list_peek_next(&g_voice_data.missed, list_node_link);
    }/* end while (this_msg_node...) */

    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, incoming = %d.\n", ordered_list_size(&g_voice_data.incoming));
    if (ordered_list_size(&g_voice_data.incoming) > 0) {
        list_node_link = ordered_list_peek_front(&g_voice_data.incoming);
    }

    while (list_node_link != NULL) {
        record_ptr = (jan_voice_call_record_t *)((jan_voice_node_list_t *)(((jan_voice_node_list_t *)list_node_link)->data))->data;

        if (record_ptr) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Incoming]contact_number = %s.\n", record_ptr->contact_number);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Incoming]call_duration = %d.\n", record_ptr->call_duration);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Incoming]call_type = %d.\n", record_ptr->call_type);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Incoming]id = %d.\n", record_ptr->id);
        }
        list_node_link =  (ordered_list_link_type *) ordered_list_peek_next(&g_voice_data.incoming, list_node_link);
    }/* end while (this_msg_node...) */

    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, outgoing = %d.\n", ordered_list_size(&g_voice_data.outgoing));
    if (ordered_list_size(&g_voice_data.outgoing) > 0) {
        list_node_link = ordered_list_peek_front(&g_voice_data.outgoing);
    }

    while (list_node_link != NULL) {
        record_ptr = (jan_voice_call_record_t *)((jan_voice_node_list_t *)(((jan_voice_node_list_t *)list_node_link)->data))->data;

        if (record_ptr) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Outgoing]contact_number = %s.\n", record_ptr->contact_number);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Outgoing]call_duration = %d.\n", record_ptr->call_duration);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Outgoing]call_type = %d.\n", record_ptr->call_type);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, [Outgoing]id = %d.\n", record_ptr->id);
        }
        list_node_link =  (ordered_list_link_type *) ordered_list_peek_next(&g_voice_data.outgoing, list_node_link);
    }/* end while (this_msg_node...) */
}
#endif


#ifdef JRD_VOICE_STATUS_DEBUG
/*===========================================================================
  Function:  get_status_string_for_debug
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
static char *get_status_string_for_debug (char *status_buf, e_jan_voice_core_status_t status)
{
    char *p_buf = status_buf;
    
    switch (status)
    {
        case E_JRD_VOICE_CORE_STATUS_IDLE:
            JRD_STRCPY (p_buf, "IDLE");
            break;
        case E_JRD_VOICE_CORE_STATUS_RING:
            JRD_STRCPY (p_buf, "RING");
            break;
        case E_JRD_VOICE_CORE_STATUS_DIALING:
            JRD_STRCPY (p_buf, "DIALING");
            break;
        case E_JRD_VOICE_CORE_STATUS_WAIT:
            JRD_STRCPY (p_buf, "WAIT");
            break;
        case E_JRD_VOICE_CORE_STATUS_ALERTING:
            JRD_STRCPY (p_buf, "ALERTING");
            break;
        case E_JRD_VOICE_CORE_STATUS_CONVERSATION:
            JRD_STRCPY (p_buf, "CONVERSATION");
            break;
        default:
            JRD_STRCPY (p_buf, "NULL");
            break;
    }
    return p_buf;
}
#endif


/*===========================================================================
  Function:  jan_voice_core_push_node
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
static void* jan_voice_core_push_node(ordered_list_type *list, int node_size, void *member, int member_size)
{
    void *new_node = NULL;

    if (!member) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, member pointer is NULL.\n");
        return new_node;
    }

    JRD_MALLOC(node_size, new_node);
    if (!new_node) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, JRD_MALLOC new node.\n");
        return new_node;
    }
    JRD_MEMCPY(((void *)new_node + (node_size - member_size)), member, member_size);

    /* All new entries will be inserted at the end of the list. */
    ordered_list_push(list, (ordered_list_link_type *)new_node, 0);
    return new_node;
}


/*===========================================================================
  Function:  jan_voice_core_pop_node
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
static int jan_voice_core_pop_node(ordered_list_type *list, void *node_ptr, int node_size, int member_size)
{
    void *member_ptr = NULL;

    if (!node_ptr) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, node pointer is NULL.\n");
        return JRD_NO_ERR;
    }

    member_ptr = (void *)node_ptr + (node_size - member_size);
    free((void *)(*(int *)member_ptr));

    ordered_list_pop_item(list, (ordered_list_link_type *)node_ptr);
    JRD_FREE(node_ptr);
    return JRD_NO_ERR;
}

/*===========================================================================
  Function:  jan_voice_core_record_limit
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
static int jan_voice_core_record_limit(int limit_num)
{
    int total_list_size = 0;
    ordered_list_link_type *list_node_link = NULL;
    ordered_list_type *record_list = NULL;
    jan_voice_node_list_t *this_record_node = NULL;
    jan_voice_call_record_t *record = NULL;

    record_list = &g_voice_data.total;
    list_node_link = ordered_list_peek_front(record_list);
    total_list_size = ordered_list_size(record_list);

    /* if g_voice_data.total list size > 100, delete front node */
    if (total_list_size > limit_num) {
        this_record_node = (jan_voice_node_list_t *)list_node_link;
        record = (jan_voice_call_record_t *)(this_record_node->data);

        #if 1
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, cur total_list_size = %d.\n", total_list_size);
        #endif

        /* Pop front node. */
        while (this_record_node->data != NULL) {
            total_list_size = ordered_list_size(record_list);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, pop total_list_size(id) = %d.\n", total_list_size);
            if (total_list_size <= limit_num) {
                break;
            }
            ordered_list_pop_front(record_list);
        }
    }

    /* Make sure 100 records in total list. */
    total_list_size = ordered_list_size(record_list);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, end total_list_size = %d.\n", total_list_size); // 100

    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_core_add_record_to_list
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
static int jan_voice_core_add_record_to_list(e_jan_voice_core_call_type_t type)
{
    int id_count = 1;
    int limit_num = JRD_VOICE_RECORD_MAX_NUM;
    struct timeval tv = {0};
    time_t curtime = {0};
    ordered_list_type *list = NULL;
    ordered_list_type *total_list = NULL;
    jan_voice_call_record_t *record_ptr = NULL;
    void *node_ptr = NULL;

    JRD_MALLOC(sizeof(jan_voice_call_record_t), record_ptr);
    if (!record_ptr) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, JRD_MALLOC new record ptr.\n");
        return JRD_FAIL;
    }

    /* Get Current Time. */
    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec + timezone_diff;

#ifdef JRD_VOICE_DEBUG
    char tmp_buf_test[32] = {0};
    strftime(tmp_buf_test, 32, "%Y-%m-%d %H:%M:%S", localtime(&curtime));
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, Test timezone_diff = %d. \n", timezone_diff);  // eg: +0800==28800
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, Test call_date = %s. \n", tmp_buf_test);
#endif

    total_list = &g_voice_data.total;
    while (JRD_VOICE_ID_MASK_BIT_SET(g_voice_data.id_mask, id_count)) {
        id_count++;
        if (id_count >= limit_num)
            break;
    }
    g_voice_data.id_mask[(id_count / 32)] |= (1 << (id_count % 32));
    record_ptr->id = id_count;

    switch (type)
    {
        case E_JRD_VOICE_CORE_CALL_TYPE_MISSED:
        {
            record_ptr->call_type = type; // 3
            record_ptr->call_date = curtime;
            record_ptr->call_duration = 0;
            /* 3rd incoming call status is WAITING and current call status is CONVERSATION. */
            if (jan_voice_al_get_third_call_state() == E_JRD_VOICE_CALL_STATE_WAITING
                && jan_voice_al_get_call_state() == E_JRD_VOICE_CALL_STATE_CONVERSATION) {
                jan_voice_al_get_third_call_number(record_ptr->contact_number);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, 3rd record_ptr->contact_number=%s.\n", record_ptr->contact_number);
            } else {
                jan_voice_al_get_call_number(record_ptr->contact_number);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, 2nd record_ptr->contact_number=%s.\n", record_ptr->contact_number);
            }

            list = &g_voice_data.missed;
            break;
        }
        case E_JRD_VOICE_CORE_CALL_TYPE_INCOMING:
        {
            record_ptr->call_type = type; // 1
            record_ptr->call_date = g_voice_data.previous_time;
            record_ptr->call_duration = (curtime - g_voice_data.previous_time);
            jan_voice_al_get_call_number(record_ptr->contact_number);

            list = &g_voice_data.incoming;
            break;
        }
        case E_JRD_VOICE_CORE_CALL_TYPE_OUTGOING:
        {
            record_ptr->call_type = type; // 2
            record_ptr->call_date = curtime;
            JRD_STRNCPY(record_ptr->contact_number, g_voice_data.call_num, g_voice_data.len);

            if (g_voice_data.is_online == 1) {
                record_ptr->call_duration = (curtime - g_voice_data.previous_time);
            } else {
                record_ptr->call_duration = 0;
            }

            list = &g_voice_data.outgoing;
            break;
        }
        default:
        {
            return JRD_NO_ERR;
        }
    }

#ifdef JRD_VOICE_DEBUG
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, contacts = %s.\n", record_ptr->contacts);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, contact_number = %s.\n", record_ptr->contact_number);

    char tmp_buf[32] = {0};
    strftime(tmp_buf, 32, "%Y-%m-%d %H:%M:%S", localtime(&record_ptr->call_date));
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_date = %s.\n", tmp_buf);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_duration = %d.\n", record_ptr->call_duration);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_type = %d.\n", record_ptr->call_type);

    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, id_count = %d.\n", id_count);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, id_mask[0] = 0x%X.\n", g_voice_data.id_mask[0]);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, id_mask[1] = 0x%X.\n", g_voice_data.id_mask[1]);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, id_mask[2] = 0x%X.\n", g_voice_data.id_mask[2]);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, id_mask[3] = 0x%X.\n", g_voice_data.id_mask[3]);
#endif

    /* Push record data to total_list. */
    node_ptr = jan_voice_core_push_node(total_list, 
                                        sizeof(jan_voice_node_list_t),
                                        &record_ptr, 
                                        sizeof(jan_voice_call_record_t *));
    if (!node_ptr) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Push record data to total list.\n");
        return JRD_FAIL;
    }
    //g_voice_data.id_count++;

    node_ptr = jan_voice_core_push_node(list, 
                                        sizeof(jan_voice_node_list_t), 
                                        &node_ptr, 
                                        sizeof(jan_voice_node_list_t *));
    if (!node_ptr) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Push node data to list.\n");
        return JRD_FAIL;
    }

    /* Delete early record when record number > 100(limit) */
    jan_voice_core_record_limit(limit_num);

    /* Save record data to datastore. */
    jan_voice_datastore_insert(record_ptr, limit_num);

#ifdef JRD_VOICE_DEBUG
    /* Only to Debug */
    jan_voice_core_show_node();
#endif
    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_core_compare_id_in_total_list
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
static int jan_voice_core_compare_id_in_total_list(void *node_ptr, void* record_id)
{
    if ((*(int *)record_id) 
        == ((jan_voice_call_record_t *)(((jan_voice_node_list_t *)node_ptr)->data))->id) {
        return 1;
    } else {
        return 0;
    }
}


/*===========================================================================
  Function:  jan_voice_core_compare_id_in_other_list
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
static int jan_voice_core_compare_id_in_other_list(void *node_ptr, void* record_id)
{
    if ((*(int *)record_id)
        == ((jan_voice_call_record_t *)((jan_voice_node_list_t *)(((jan_voice_node_list_t *)node_ptr)->data))->data)->id) {
        return 1;
    } else {
        return 0;
    }
}


/*===========================================================================
  Function:  jan_voice_core_compare_member_ptr
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
static int jan_voice_core_compare_member_ptr(void *node_ptr, void* member_ptr)
{
    if ((jan_voice_node_list_t *)(((jan_voice_node_list_t *)node_ptr)->data) == member_ptr) {
        return 1;
    } else {
        return 0;
    }
}


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
static int jan_voice_core_delete_record_from_list(ordered_list_type *list, int id)
{
    jan_voice_node_list_t *node_ptr = NULL;
    jan_voice_node_list_t *tmp_node_ptr = NULL;
    ordered_list_type *tmp_list = NULL;
    int rc = JRD_NO_ERR;

    if (!list) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, list pointer is NULL.\n");
        return JRD_FAIL;
    }

    if (list == &g_voice_data.total) {  /* Delete record from total list. */
        node_ptr = ordered_list_linear_search(list, jan_voice_core_compare_id_in_total_list, &id);
        if (!node_ptr) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Not find record id:%d in total list.\n", id);
            return JRD_FAIL;
        }

        switch (((jan_voice_call_record_t *)(node_ptr->data))->call_type)
        {
            case E_JRD_VOICE_CORE_CALL_TYPE_INCOMING:
            {
                tmp_list = &g_voice_data.incoming;
                break;
            }
            case E_JRD_VOICE_CORE_CALL_TYPE_OUTGOING:
            {
                tmp_list = &g_voice_data.outgoing;
                break;
            }
            case E_JRD_VOICE_CORE_CALL_TYPE_MISSED:
            {
                tmp_list = &g_voice_data.missed;
                break;
            }
            default:
            {
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Unknown call type.\n");
                return JRD_FAIL;
            }
        }

        tmp_node_ptr = ordered_list_linear_search(tmp_list, jan_voice_core_compare_member_ptr, node_ptr);
        if (!tmp_node_ptr) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Not find member ptr.\n");
            return JRD_FAIL;
        }
        ordered_list_pop_item(tmp_list, (ordered_list_link_type *)tmp_node_ptr);
        JRD_FREE(tmp_node_ptr);

        rc = jan_voice_core_pop_node(&g_voice_data.total, node_ptr, sizeof(jan_voice_node_list_t), sizeof(jan_voice_node_list_t *));
        if (rc != JRD_NO_ERR) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Cann't delete record from total list, id:%d.\n", id);
            return JRD_FAIL;
        }
    } else {  /* Delete record from other list, such as missed/incoming/outgoing. */
        node_ptr = ordered_list_linear_search(list, jan_voice_core_compare_id_in_other_list, &id);
        if (!node_ptr) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Not find record id:%d in other list.\n", id);
            return JRD_FAIL;
        }

        rc = jan_voice_core_pop_node(&g_voice_data.total, node_ptr->data, sizeof(jan_voice_node_list_t), sizeof(jan_voice_node_list_t *));
        if (rc != JRD_NO_ERR) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Cann't delete record from list, id:%d.\n", id);
            return JRD_FAIL;
        }
        ordered_list_pop_item(list, (ordered_list_link_type *)node_ptr);
        JRD_FREE(node_ptr);
    }

    /* Delete record data from datastore. */
    jan_voice_datastore_delete(id);
    g_voice_data.id_mask[(id / 32)] &= (~(1 << (id % 32)));

    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_core_clear_list
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
static int jan_voice_core_clear_record_from_list(ordered_list_type *list)
{

    /* Clear record data from datastore. */
    jan_voice_datastore_clear();

    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_core_init_record_form_datastore
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
static int jan_voice_core_init_record_form_datastore(int n_count, char** column_value, char** column_name)
{
    jan_voice_call_record_t *record_ptr = NULL;
    ordered_list_type *list = NULL;
    void *node_ptr = NULL;
    int n_len = 0;
    int index = 0;

    JRD_MALLOC(sizeof(jan_voice_call_record_t), record_ptr);
    if (!record_ptr) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, JRD_MALLOC new record ptr.\n");
        return JRD_FAIL;
    }

    #ifdef JRD_VOICE_DEBUG
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, n_count = %d.\n", n_count);
    #endif
    for (index = 0; index < n_count; index++) {
        if (0 == JRD_STRCMP(column_name[index] , "ID")) {
            JRD_SSCANF(column_value[index], "%d", &record_ptr->id);
        } else if (0 == JRD_STRCMP(column_name[index] , "Contacts")) {
            /* Nothing to do. */
        } else if (0 == JRD_STRCMP(column_name[index] , "ContactNumber")) {
            n_len = (sizeof(record_ptr->contact_number) / sizeof(char));
            if (JRD_STRLEN(column_value[index]) <= n_len) {
                JRD_STRCPY(record_ptr->contact_number, column_value[index]);
            }
        } else if (0 == JRD_STRCMP(column_name[index] , "Date")) {
            JRD_SSCANF(column_value[index], "%d", &record_ptr->call_date);
        }  else if (0 == JRD_STRCMP(column_name[index] , "Duration")) {
            JRD_SSCANF(column_value[index], "%d", &record_ptr->call_duration);
        } else if (0 == JRD_STRCMP(column_name[index] , "Type")) {
            JRD_SSCANF(column_value[index], "%d", &record_ptr->call_type);
        } else {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Cann't parse column name.\n");
            return JRD_FAIL;
        }
    }

    switch (record_ptr->call_type)
    {
        case E_JRD_VOICE_CORE_CALL_TYPE_MISSED:
        {
            list = &g_voice_data.missed;
            break;
        }
        case E_JRD_VOICE_CORE_CALL_TYPE_INCOMING:
        {
            list = &g_voice_data.incoming;
            break;
        }
        case E_JRD_VOICE_CORE_CALL_TYPE_OUTGOING:
        {
            list = &g_voice_data.outgoing;
            break;
        }
        default:
        {
            return JRD_NO_ERR;
        }
    }

    /* Push record data to total_list. */
    node_ptr = jan_voice_core_push_node(&g_voice_data.total, 
                                        sizeof(jan_voice_node_list_t), 
                                        &record_ptr, 
                                        sizeof(jan_voice_call_record_t *));
    if (!node_ptr) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Push record data to total list.\n");
        return JRD_FAIL;
    }

    node_ptr = jan_voice_core_push_node(list, 
                                        sizeof(jan_voice_node_list_t), 
                                        &node_ptr, 
                                        sizeof(jan_voice_node_list_t *));
    if (!node_ptr) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Push node data to list.\n");
        return JRD_FAIL;
    }

    if (0 == JRD_VOICE_ID_MASK_BIT_SET(g_voice_data.id_mask, record_ptr->id)) {
        g_voice_data.id_mask[(record_ptr->id / 32)] |= (1 << (record_ptr->id % 32));
        #ifdef JRD_VOICE_DEBUG
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, record_ptr->id = %d.\n", record_ptr->id);
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, (1 << (record_ptr->id % 32)) = 0x%X.\n", (1 << (record_ptr->id % 32)));
        #endif
    }

#ifdef JRD_VOICE_DEBUG
    char tmp_buf[32] = {0};

    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, contact_number = %s.\n", record_ptr->contact_number);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_duration = %d.\n", record_ptr->call_duration);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_type = %d.\n", record_ptr->call_type);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, id = %d.\n", record_ptr->id);

    strftime(tmp_buf, 32, "%Y%m%d_%H%M%S", localtime(&record_ptr->call_date));
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_date = %s.\n", tmp_buf);

    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, id_mask[0] = 0x%X.\n", g_voice_data.id_mask[0]);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, id_mask[1] = 0x%X.\n", g_voice_data.id_mask[1]);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, id_mask[2] = 0x%X.\n", g_voice_data.id_mask[2]);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, id_mask[3] = 0x%X.\n", g_voice_data.id_mask[3]);
#endif

    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_core_indication_to_led
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
static int jan_voice_core_indication_to_led(e_jan_voice_core_status_t late_status)
{
    jan_ind_data_type ind_info = {0};
    int call_status;

    switch(late_status)
    {
        case E_JRD_VOICE_CORE_STATUS_IDLE:
        {
            ind_info.ind_id = E_JRD_VOICE_IND_CALL_STATUS_CHANGE;
            call_status = E_JRD_VOICE_IND_CALL_IDLE;
            break;
        }
        case E_JRD_VOICE_CORE_STATUS_RING:
        {
            ind_info.ind_id = E_JRD_VOICE_IND_CALL_STATUS_CHANGE;
            call_status = E_JRD_VOICE_IND_CALL_RING;
            break;
        }
        case E_JRD_VOICE_CORE_STATUS_CONVERSATION:
        case E_JRD_VOICE_CORE_STATUS_DIALING:
        case E_JRD_VOICE_CORE_STATUS_WAIT:
        case E_JRD_VOICE_CORE_STATUS_ALERTING:
        {
            ind_info.ind_id = E_JRD_VOICE_IND_CALL_STATUS_CHANGE;
            call_status = E_JRD_VOICE_IND_CALL_CONVERSATION;
            break;
        }
        default:
        {
            break;
        }
    }
    ind_info.ind_data = (void*)&call_status;
    ind_info.data_size = sizeof(int);
    ind_info.module_id = MODULE_VOICE;
    jan_oem_ind(&ind_info);
    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_core_split_emergency_number
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

static int jan_voice_core_split_emergency_number(char *src_str, char dest_str[][JRD_VOICE_EMERGENCY_NUM_LEN])
{
  int index = 0;
  char *ph = src_str;
  char *p  = src_str;

  if(!src_str)
    return JRD_FAIL;

  while(*p && isspace(*p) == 0)
  {
    if(index > JRD_VOICE_EMERGENCY_NUM)
      return JRD_FAIL;

    if(*(++p) == ',' || *p == '\0' || isspace(*p))
    {
      JRD_MEMCPY(dest_str[index], ph, (p - ph));
      index++;
      ph = p+1;
    }
  }
  return JRD_NO_ERR;

}

/*===========================================================================
  FUNCTION  jan_voice_core_check_ecc_number
===========================================================================*/
/*!
@brief
    Check call_numer is the emergency number or not.

@return
  0,-1

*/
/*=========================================================================*/
static int jan_voice_core_check_ecc_number(char *call_number)
{
    char database_emergency_number[32] = {0};
    char emergency_number[JRD_VOICE_EMERGENCY_NUM][JRD_VOICE_EMERGENCY_NUM_LEN] = {0};
    int rs = JRD_NO_ERR;
    int i = 0;

    jan_voice_get_emergency_number (database_emergency_number);
    rs = jan_voice_core_split_emergency_number (database_emergency_number, emergency_number);
    if (rs != JRD_NO_ERR) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Error, split emergency number failed.\n");
        return JRD_FAIL;
    }

#ifdef JRD_VOICE_DEBUG
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, database_emergency_number = %s\n", database_emergency_number);
    for (i = 0; i < JRD_VOICE_EMERGENCY_NUM && JRD_STRLEN(emergency_number[i]); i++) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, emergency_number[%d] = %s.\n", i, emergency_number[i]);
    }
#endif

    for (i = 0; i < JRD_VOICE_EMERGENCY_NUM && JRD_STRLEN (emergency_number[i]); i++) {
        if (JRD_STRCMP (call_number, emergency_number[i]) == 0) {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, It's emergency numer: %s .\n", call_number);
            return 0;
        }
    }

    return -1;
}



/*===========================================================================
  Function:  jan_voice_core_dialing_out
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
static void jan_voice_core_dialing_out(int param)
{
    e_jan_voice_dial_type_t dial_type = E_JRD_VOICE_DIAL_TYPE_INVALID;
    int result = 0;

    if (g_voice_data.late_status == E_JRD_VOICE_CORE_STATUS_DIALING) {

        #ifdef JRD_VOICE_DEBUG
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"**********************************************************\n");
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_num = %s, len = %d.\n", g_voice_data.call_num, g_voice_data.len);
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"**********************************************************\n");
        #endif

        result = jan_voice_core_check_ecc_number (g_voice_data.call_num);
        if (result == 0) {
            dial_type = E_JRD_VOICE_DIAL_TYPE_EMERGENCY;
        } else {
            dial_type = E_JRD_VOICE_DIAL_TYPE_NORMAL;
        }
        jan_voice_al_call_dial(g_voice_data.call_num, dial_type);
    } else {
        /* Disable oscillator. */
        jan_slic_opt_oscillator(g_voice_data.slic_dev, 0);
        g_voice_data.curr_status = E_JRD_VOICE_CORE_STATUS_IDLE;
        g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_IDLE;
    }
}

#if 0
/*===========================================================================
  Function:  jan_voice_core_timeout_busy_tone
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
static void jan_voice_core_timeout_busy_tone(int param)
{
    jan_slic_set_tone_osc_time (g_voice_data.slic_dev, 350, 350);
    jan_slic_opt_pcm(g_voice_data.slic_dev, 1);                /* Enable PCM. */

    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, timeout busy tone play, disable pcm...\n");
}
#endif


/*===========================================================================
  Function:  jan_voice_core_wait_timeout
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
static int jan_voice_core_wait_timeout(void *param)
{
    jan_slic_device_t *dev = (jan_slic_device_t *)param;

    if (!dev) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, timer param is NULL.\n");
        return JRD_FAIL;
    }

    if (g_voice_data.late_status == E_JRD_VOICE_CORE_STATUS_DIALING) {
        /* Get Call Number. */
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_num = %s, len = %d.\n", g_voice_data.call_num, g_voice_data.len);
    } else {
        /* Disable oscillator. */
        jan_slic_opt_oscillator(g_voice_data.slic_dev, 0);
        g_voice_data.curr_status = E_JRD_VOICE_CORE_STATUS_IDLE;
        g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_IDLE;
    }

    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_core_create_device
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
static jan_slic_device_t *jan_voice_core_create_device(void)
{
    jan_slic_device_t *dev = NULL;
    int rc = 0;

    dev = jan_slic_allocate_device();
    if (!dev) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, allocate slic device.\n");
        return NULL;
    }

    rc = jan_slic_register_device(dev, JRD_VOICE_SLIC_SPIDEV);
    if (rc != 0) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, register slic device.\n");
        return NULL;
    }

    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, dev = %p.\n", dev);
    return dev;
}

/*===========================================================================
  Function:  jan_voice_core_status_handle
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
static int jan_voice_core_status_handle(e_jan_voice_core_status_t curr_status, e_jan_voice_core_status_t late_status)
{
    //e_jan_slic_device_t state = E_JRD_SLIC_DEVICE_UNKNOWN;
    char call_name[64] = {0};
    char call_number[32] = {0};
    char call_date[32] = {0};
    char curr_buf[20] = {0};
    char late_buf[20] = {0};
    struct timeval tv = {0};
    time_t time = 0;
    int rc = 0;

    #ifdef JRD_VOICE_STATUS_DEBUG
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "enter status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
    #endif
    
    /* Indication call statue to led. */
    jan_voice_core_indication_to_led(late_status);

    switch (late_status)
    {
        case E_JRD_VOICE_CORE_STATUS_IDLE:
        {
            if (curr_status == E_JRD_VOICE_CORE_STATUS_RING) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif
                jan_voice_core_add_record_to_list(E_JRD_VOICE_CORE_CALL_TYPE_MISSED);

                g_voice_data.dev_state = E_JRD_SLIC_DEVICE_UNKNOWN;
                jan_slic_opt_ring(g_voice_data.slic_dev, 0);                 /* Disable Ring. */
                jan_slic_opt_pcm(g_voice_data.slic_dev, 0);                  /* Disable PCM. */
                jan_slic_opt_oscillator(g_voice_data.slic_dev, 0);           /* Disable oscillator. */
                jan_voice_al_call_end();                                     /* End Call. */
                jan_slic_set_tone_osc_time(g_voice_data.slic_dev, 0, 0);     /* End Tone. */
            } else if (curr_status == E_JRD_VOICE_CORE_STATUS_CONVERSATION) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif
                if (g_voice_data.is_incoming == 1) {
                    jan_voice_core_add_record_to_list(E_JRD_VOICE_CORE_CALL_TYPE_INCOMING);
                } else {
                    jan_voice_core_add_record_to_list(E_JRD_VOICE_CORE_CALL_TYPE_OUTGOING);
                }
                jan_voice_al_call_end();                                     /* End Call. */
                jan_slic_opt_pcm(g_voice_data.slic_dev, 0);                 /* Disable PCM. */
                jan_slic_set_tone_osc_time(g_voice_data.slic_dev, 0, 0);  /* End Tone. */
            } else if (curr_status == E_JRD_VOICE_CORE_STATUS_ALERTING) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif
                jan_voice_core_add_record_to_list(E_JRD_VOICE_CORE_CALL_TYPE_OUTGOING);

                jan_voice_al_call_end();                                    /* End Call. */
                jan_slic_opt_pcm(g_voice_data.slic_dev, 0);                 /* Disable PCM. */
                jan_slic_set_tone_osc_time(g_voice_data.slic_dev, 0, 0);    /* End Tone. */
            } else if (curr_status == E_JRD_VOICE_CORE_STATUS_DIALING) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif
                jan_timer_stop(&(g_voice_data.call_timer));             /* Stop 15s Wait Time. */
            } else if (curr_status == E_JRD_VOICE_CORE_STATUS_WAIT) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif
                jan_timer_stop(&(g_voice_data.call_timer));             /* Stop 15s Wait Time. */
                jan_slic_opt_oscillator(g_voice_data.slic_dev, 0);      /* Disable oscillator. */
                jan_slic_set_tone_osc_time(g_voice_data.slic_dev, 0, 0);        /* Close busy tone. */
            } else {
                /* Nothing to do. */
                return JRD_NO_ERR;
            }

            /* Clear call-out number and current call record. */
            JRD_MEMSET(&g_voice_data.call_num, 0, sizeof(g_voice_data.call_num));
            g_voice_data.previous_time = 0;
            g_voice_data.is_incoming = 0;
            g_voice_data.is_online = 0;
            g_voice_data.len = 0;
            g_voice_data.tone_flag = 0;
            break;
        }
        case E_JRD_VOICE_CORE_STATUS_RING:
        {
            if (curr_status == E_JRD_VOICE_CORE_STATUS_IDLE) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif

                /* Get current time and convert time format. */
                gettimeofday(&tv, NULL);
                time = tv.tv_sec + timezone_diff;
                strftime(call_date, 32, "%m%d%H%M", localtime(&time));
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_date = %s.\n", call_date);

                /* For 3rd call CID display. */
                if (jan_voice_al_get_call_state() == E_JRD_VOICE_CALL_STATE_DISCONNECTING
                    || jan_voice_al_get_call_state() == E_JRD_VOICE_CALL_STATE_END) {
                    jan_voice_al_get_third_call_number(call_number);
                } else {
                    jan_voice_al_get_call_number(call_number);
                }

                jan_slic_opt_ring(g_voice_data.slic_dev, 1);    /* Enable Ring. */

                /* Display CID -- Caller number. */
                //rc = jan_slic_opt_send_CID(g_voice_data.slic_dev, call_name, call_number, call_date);
                rc = jan_slic_opt_send_CID(g_voice_data.slic_dev, call_name, call_number, call_date, &(g_voice_data.dev_state));
                JRD_OEM_LOG_INFO (JRD_OEM_LOG_ERROR, "Info, g_voice_data.dev_state = %d\n", g_voice_data.dev_state);
                if (rc != JRD_NO_ERR) {
                    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Send CID.\n");
                    return JRD_FAIL;
                }
            }
            if (curr_status == E_JRD_VOICE_CORE_STATUS_RING) {
                /* Nothing to do. */
            }
            break;
        }
        case E_JRD_VOICE_CORE_STATUS_CONVERSATION:
        {
            g_voice_data.is_online = 1;
            if (curr_status == E_JRD_VOICE_CORE_STATUS_RING) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif
                g_voice_data.is_incoming = 1;

                /* Get Current Time. */
                gettimeofday(&tv, NULL);
                g_voice_data.previous_time = tv.tv_sec + timezone_diff;
                char call_date_test1[32] = {0};
                strftime(call_date_test1, 32, "%m%d%H%M", localtime(&g_voice_data.previous_time));
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info[yuan.jiang1], Test timezone_diff = %d. \n", timezone_diff);
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_date_test1 = %s.\n", call_date_test1);

                jan_slic_opt_ring(g_voice_data.slic_dev, 0);    /* Disable Ring. */
                jan_slic_opt_pcm(g_voice_data.slic_dev, 1);     /* Enable PCM. */
                jan_voice_al_call_answer();                      /* Answer call. */
            } else if (curr_status == E_JRD_VOICE_CORE_STATUS_ALERTING) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif

                /* Get Current Time. */
                gettimeofday(&tv, NULL);
                g_voice_data.previous_time = tv.tv_sec + timezone_diff;
                char call_date_test2[32] = {0};
                strftime(call_date_test2, 32, "%m%d%H%M", localtime(&g_voice_data.previous_time));
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_date_test2 = %s.\n", call_date_test2);

                jan_slic_opt_pcm(g_voice_data.slic_dev, 1);     /* Enable PCM. */
            } else {
                return JRD_NO_ERR;
            }
            break;
        }
        case E_JRD_VOICE_CORE_STATUS_WAIT:
        {
            if (curr_status == E_JRD_VOICE_CORE_STATUS_IDLE) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif
                jan_timer_start(&(g_voice_data.call_timer));           /* 15s Wait Time. */
                jan_slic_opt_oscillator(g_voice_data.slic_dev, 1);    /* Enable oscillator. */
            }
            break;
        }
        case E_JRD_VOICE_CORE_STATUS_DIALING:
        {
            if (curr_status == E_JRD_VOICE_CORE_STATUS_WAIT) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif
                jan_timer_stop (&(g_voice_data.call_timer));
                jan_slic_opt_oscillator(g_voice_data.slic_dev, 0);    /* Disable oscillator */
            } else if (curr_status == E_JRD_VOICE_CORE_STATUS_DIALING) {
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, user is pressing digit.\n");
            } else {
                /* Nothing to do. */
            }
            break;
        }
        case E_JRD_VOICE_CORE_STATUS_ALERTING:
        {
            if (curr_status == E_JRD_VOICE_CORE_STATUS_DIALING) {
                #ifdef JRD_VOICE_STATUS_DEBUG
                JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "status change : %s >>> %s\n", get_status_string_for_debug (curr_buf, curr_status), get_status_string_for_debug (late_buf, late_status));
                #endif
                jan_slic_opt_pcm(g_voice_data.slic_dev, 1);                /* Enable PCM. */
                //alarm (JRD_VOICE_CALLOUT_TIMEOUT);
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return JRD_NO_ERR;
}


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
int jan_voice_core_delete_record_id(int list_type, int record_id)
{
    ordered_list_type *list = NULL;
    int rc = JRD_NO_ERR;

    if (list_type == E_JRD_VOICE_LIST_TYPE_ALL) {
        list = &g_voice_data.total;
    } else if (list_type == E_JRD_VOICE_LIST_TYPE_INCOMING) {
        list = &g_voice_data.incoming;
    } else if (list_type == E_JRD_VOICE_LIST_TYPE_OUTGOING) {
        list = &g_voice_data.outgoing;
    } else if (list_type == E_JRD_VOICE_LIST_TYPE_MISSED) {
        list = &g_voice_data.missed;
    } else {
        return JRD_FAIL;
    }

    rc = jan_voice_core_delete_record_from_list(list, record_id);

    return JRD_NO_ERR;
}


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
int jan_voice_core_clear_record_list(ordered_list_type *record_list)
{
    int rc = JRD_NO_ERR;

    if (!record_list) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, record list pointer is NULL.\n");
        return JRD_FAIL;
    }

    rc = jan_voice_core_clear_record_from_list(record_list);
    if (rc != JRD_NO_ERR) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Cann't clear record list.\n");
        return JRD_FAIL;
    }

    return JRD_NO_ERR;
}


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
jan_voice_call_record_count_info_t* jan_voice_core_get_record_count_info(void)
{

    g_voice_data.count_info.incoming_count = ordered_list_size(&g_voice_data.incoming);
    g_voice_data.count_info.outgoing_count = ordered_list_size(&g_voice_data.outgoing);
    g_voice_data.count_info.missed_count = ordered_list_size(&g_voice_data.missed);
    g_voice_data.count_info.total_count = ordered_list_size(&g_voice_data.total);
    return &g_voice_data.count_info;
}


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
ordered_list_type* jan_voice_core_get_record_list(e_jan_voice_list_type_t list_type)
{
    ordered_list_type *list = NULL;

    if (list_type == E_JRD_VOICE_LIST_TYPE_ALL) {
        list = &g_voice_data.total;
    } else if (list_type == E_JRD_VOICE_LIST_TYPE_INCOMING) {
        list = &g_voice_data.incoming;
    } else if (list_type == E_JRD_VOICE_LIST_TYPE_OUTGOING) {
        list = &g_voice_data.outgoing;
    } else if (list_type == E_JRD_VOICE_LIST_TYPE_MISSED) {
        list = &g_voice_data.missed;
    } else {
        return NULL;
    }

    return list;
}


#if 0
/*===========================================================================
  Function:  jan_voice_core_correct_time
===========================================================================*/
/*!
@brief
  Correct time when device init.

@parameters
  None.

@return
  None.
*/
/*=========================================================================*/
int jan_voice_core_correct_time(void)
{
    char call_name[64] = {0};
    char call_number[32] = {0};
    char call_date[32] = {0};
    struct timeval tv = {0};
    time_t time = 0;
    int rc = 0;

    /* Get current time and convert time format. */
    gettimeofday(&tv, NULL);
    time = tv.tv_sec + timezone_diff;
    strftime(call_date, 32, "%m%d%H%M", localtime(&time));

    /* Corrention time. */
    rc = jan_slic_opt_send_CID(g_voice_data.slic_dev, call_name, call_number, call_date, &(g_voice_data.dev_state));
    if (rc != JRD_NO_ERR) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, Send correction time.\n");
        return JRD_FAIL;
    }

}
#endif


/*===========================================================================
  Function:  jan_voice_core_timezone_handle
===========================================================================*/
/*!
@brief
  Set time by the timezone.

@parameters
  None.

@return
  None.
*/
/*=========================================================================*/
int jan_voice_core_timezone_handle(jan_voice_timezone_data_t *time_info)
{
    char tmp_time_diff[10] = {0};
    char *p_time_diff = NULL;
    int number = 0;
    int hours = 0;
    int minutes = 0;
    //long seconds_diff = 0;
    int seconds_diff = 0;
    

    if (NULL == time_info->time_diff || NULL == time_info->time_zone || NULL == time_info->time_zone_name)
    {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, time_info is NULL!!! \n");
        return JRD_FAIL;
    }

#ifdef JRD_VOICE_DEBUG
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, voice time_zone_name: %s. \n", time_info->time_zone_name);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, voice time_zone: %s. \n", time_info->time_zone);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, voice time_diff: %s. \n", time_info->time_diff);
#endif

    /* Change time_diff to number. */
    JRD_STRNCPY (tmp_time_diff, time_info->time_diff, JRD_STRLEN (time_info->time_diff));
    p_time_diff = &tmp_time_diff[1];
    number = JRD_ATOI(p_time_diff);
    hours = number / 100;
    minutes = number % 100;

    seconds_diff = (hours * 3600) + (minutes * 60);

    /* Judge + - symbol. */
    if ('+' == tmp_time_diff[0])
    {
        timezone_diff = seconds_diff;
    }
    else if ('-' == tmp_time_diff[0])
    {
        timezone_diff = 0 - seconds_diff;
    }
    else
    {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, timezone +- symbol error!!!\n");
        return JRD_FAIL;
    }
    
#ifdef JRD_VOICE_DEBUG
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, timezone_diff: %d. \n", timezone_diff);
#endif

#if 0
    struct timeval tv = {0};
    time_t time = 0;
    char timezone_date[32] = {0};
    /* Set current time and convert time format. */
    settimeofday(&tv, NULL);
    time = tv.tv_sec + timezone_diff;
    strftime(timezone_date, 32, "%m%d%H%M", localtime(&time));
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, timezone_date = %s.\n", timezone_date);
#endif
    //jan_voice_core_correct_time ();
    return JRD_NO_ERROR;
}


/*===========================================================================
  Function:  jan_voice_core_device_event_handle
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
int jan_voice_core_device_event_handle(void)
{
    int rc = 0;
    char dtmf_number = NULL;
    e_jan_slic_device_t state = E_JRD_SLIC_DEVICE_UNKNOWN;

    sighandler_t alrm_handler = jan_voice_core_dialing_out;
    signal (SIGALRM, alrm_handler);

#if 0
    jan_slic_get_device_state(g_voice_data.slic_dev, &state);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, state = %d.\n", state);
#else

    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, curr_status=%d, late_status=%d\n", g_voice_data.curr_status, g_voice_data.late_status);
    if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_RING
        && g_voice_data.dev_state == E_JRD_SLIC_DEVICE_OFFHOOK) {
        state = g_voice_data.dev_state;
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, g_state = %d.\n", state);
    } else {
        /* For normal calling. */
        jan_slic_get_device_state(g_voice_data.slic_dev, &state);
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, state = %d.\n", state);
    }
#endif

    switch (state)
    {
        case E_JRD_SLIC_DEVICE_ONHOOK:
        {
            /* Clear dtmf queue. */
            jan_dtmf_queue.head = 0;
            jan_dtmf_queue.tail = 0;

            /* For 3rd call wake up after current calling end. */
            if (jan_voice_al_get_third_call_state() == E_JRD_VOICE_CALL_STATE_INCOMING) {
                g_voice_data.curr_status = E_JRD_VOICE_CORE_STATUS_IDLE;
                g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_RING;
                jan_voice_al_clear_third_call_state();
            } else if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_RING) {
                /* Nothing to do. For hook change at RING status. */
            } else {
                g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_IDLE;
            }
            break;
        }
        case E_JRD_SLIC_DEVICE_OFFHOOK:
        {
            if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_IDLE) {
                g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_WAIT;
            } else if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_RING) {
                g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_CONVERSATION;
            } else {
                g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_IDLE;
            }
            break;
        }

        case E_JRD_SLIC_DEVICE_DIGIT:
        {
            if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_WAIT
                || g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_DIALING) {
                g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_DIALING;

                /* Max call number is 32-1 length. */
                if (g_voice_data.len < JRD_VOICE_CALL_OUT_NUMBER-1)
                {
                    rc = jan_slic_opt_read_digit(g_voice_data.slic_dev, &g_voice_data.call_num[g_voice_data.len]);
                    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Call_num: %d. len: %d. \n", g_voice_data.call_num[g_voice_data.len], g_voice_data.len+1);
                    if (rc != 0)
                    {
                        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, read device digit.\n");
                        return JRD_FAIL;
                    }
                    
                    g_voice_data.len++;
                    
                    /* Every number has 5s alarm. */
                    alarm (JRD_VOICE_DIALING_OUT_TIME);
                }
            }
            else if (g_voice_data.curr_status = E_JRD_VOICE_CORE_STATUS_CONVERSATION)
            {
                /* Max dtmf number is 64-1 length. */
                if (jan_dtmf_queue.tail < JRD_VOICE_DTMF_NUMBER_LEN-1)
                {
                    rc = jan_slic_opt_read_digit(g_voice_data.slic_dev, &jan_dtmf_queue.dtmf_num[jan_dtmf_queue.tail]);
                    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"--->>[DTMF]<<--- num: %d. len: %d. \n", jan_dtmf_queue.dtmf_num[jan_dtmf_queue.tail], jan_dtmf_queue.tail+1);
                    if (rc != 0)
                    {
                        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Error, read device digit of dtmf.\n");
                        return JRD_FAIL;
                    }
                    jan_dtmf_queue.tail++;
#if 1
                    if (jan_dtmf_queue.dtmf_num[jan_dtmf_queue.head])
                    {
                        pthread_cond_signal (&jan_key_signal.cond);
                        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, DTMF signal sent >>.\n");
                    }
#endif
                    g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_CONVERSATION;
                }
            }
            break;
        }
        default:
        {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, slic state = %d.\n", state);
            //break;
            return JRD_NO_ERR;
        }
    }

    /* Handle voice status. */
    if (g_voice_data.curr_status != g_voice_data.late_status) {

        jan_voice_core_status_handle(g_voice_data.curr_status, g_voice_data.late_status);
        g_voice_data.curr_status = g_voice_data.late_status;
    }
    return JRD_NO_ERR;
}


/*===========================================================================
  Function:  jan_voice_core_network_event_handle
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
int jan_voice_core_network_event_handle(jan_voice_call_info_t *call_info)
{
    jan_voice_socket_msg_t msg = {0};
    int result = JRD_FAIL; // -1
    char call_name[64] = {0};
    char call_number[32] = {0};
    char call_date[32] = {0};
    struct timeval tv = {0};
    time_t time = 0;
    int rc = 0;

#ifdef JRD_VOICE_DEBUG
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"********************************************************\n");
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, call_state = %d, call_id = %d, call_number = %s.\n", call_info->call_state, call_info->call_id, call_info->call_number);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"********************************************************\n");
#endif

    switch (call_info->call_state)
    {
        case E_JRD_VOICE_CALL_STATE_SETUP:
        {
            break;
        }
        case E_JRD_VOICE_CALL_STATE_INCOMING:
        {
            #ifdef JRD_VOICE_DEBUG
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "<INCOMING>Info, curr_status=%d, late_status=%d.\n", g_voice_data.curr_status, g_voice_data.late_status);
            #endif

            if (call_info->call_mode == E_JRD_VOICE_CALL_MODE_LTE
                && (call_info->call_type == E_JRD_VOICE_CALL_TYPE_VOICE_IP
                || call_info->call_type == E_JRD_VOICE_CALL_TYPE_EMERGENCY_IP)) {
                /* Switch Voice path to VoLTE. */
                msg.sock_opt = E_JRD_VOICE_SOCKET_OPT_SET;
                JRD_STRCPY(msg.device, JRD_VOICE_SOUNDCARD_VOLTE_DEVICE);

                jan_voice_connect_send_msg(E_JRD_VOICE_SOCKET_SERVER_TYPE_APLAY, &msg);
                jan_voice_connect_send_msg(E_JRD_VOICE_SOCKET_SERVER_TYPE_AREC, &msg);
            } else if ((call_info->call_type == E_JRD_VOICE_CALL_TYPE_VOICE 
                || call_info->call_type == E_JRD_VOICE_CALL_TYPE_EMERGENCY)) {
                /* Switch Voice path to CSVoice. */
                msg.sock_opt = E_JRD_VOICE_SOCKET_OPT_SET;
                JRD_STRCPY(msg.device, JRD_VOICE_SOUNDCARD_CSVOICE_DEVICE);

                jan_voice_connect_send_msg(E_JRD_VOICE_SOCKET_SERVER_TYPE_APLAY, &msg);
                jan_voice_connect_send_msg(E_JRD_VOICE_SOCKET_SERVER_TYPE_AREC, &msg);
            } else {
                    /* Nothing to do. */
            }
/*
            jan_slic_get_device_state(g_voice_data.slic_dev, &state);
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, slic state = %d.\n", state);
            if (state == E_JRD_SLIC_DEVICE_OFFHOOK || state == E_JRD_SLIC_DEVICE_DIGIT)
            {
                result = jan_voice_al_call_answer_busy();
            }
*/
            if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_IDLE)
            {
                g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_RING;
            }

            break;
        }
        case E_JRD_VOICE_CALL_STATE_CC_IN_PROGRESS:
        case E_JRD_VOICE_CALL_STATE_ORIGINATING:
        case E_JRD_VOICE_CALL_STATE_ALERTING:
        case E_JRD_VOICE_CALL_STATE_WAITING:
        {
            #ifdef JRD_VOICE_DEBUG
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "<WAITING>Info, curr_status=%d, late_status=%d.\n", g_voice_data.curr_status, g_voice_data.late_status);
            #endif

            if (call_info->call_state == E_JRD_VOICE_CALL_STATE_ALERTING) {
                if (call_info->call_mode == E_JRD_VOICE_CALL_MODE_LTE
                    && (call_info->call_type == E_JRD_VOICE_CALL_TYPE_VOICE_IP
                    || call_info->call_type == E_JRD_VOICE_CALL_TYPE_EMERGENCY_IP)) {
                    /* Switch Voice path to VoLTE. */
                    msg.sock_opt = E_JRD_VOICE_SOCKET_OPT_SET;
                    JRD_STRCPY(msg.device, JRD_VOICE_SOUNDCARD_VOLTE_DEVICE);

                    jan_voice_connect_send_msg(E_JRD_VOICE_SOCKET_SERVER_TYPE_APLAY, &msg);
                    jan_voice_connect_send_msg(E_JRD_VOICE_SOCKET_SERVER_TYPE_AREC, &msg);
                } else if ((call_info->call_type == E_JRD_VOICE_CALL_TYPE_VOICE
                    || call_info->call_type == E_JRD_VOICE_CALL_TYPE_EMERGENCY)) {
                    /* Switch Voice path to CSVoice. */
                    msg.sock_opt = E_JRD_VOICE_SOCKET_OPT_SET;
                    JRD_STRCPY(msg.device, JRD_VOICE_SOUNDCARD_CSVOICE_DEVICE);

                    jan_voice_connect_send_msg(E_JRD_VOICE_SOCKET_SERVER_TYPE_APLAY, &msg);
                    jan_voice_connect_send_msg(E_JRD_VOICE_SOCKET_SERVER_TYPE_AREC, &msg);
                } else {
                    /* Nothing to do. */
                }
            }
            else {
                if (g_voice_data.tone_flag < 1) {
                    if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_CONVERSATION
                        && jan_voice_al_get_third_call_state() == E_JRD_VOICE_CALL_STATE_WAITING)
                    {
                        /* Make sure 3rd tone play only once, because WAITING state occur twice. */
                        g_voice_data.tone_flag++;
                        jan_slic_set_tone_osc_time(g_voice_data.slic_dev, 250, 1000);
                        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "Info, 3rd call tone osc 250ms-1000ms...play!!\n");

                        //jan_voice_al_call_answer_busy();

                        /* Only add record MISSED number once, for 3rd incoming call_number. */
                        jan_voice_core_add_record_to_list(E_JRD_VOICE_CORE_CALL_TYPE_MISSED);
                    }
                }
            }

            if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_DIALING) {
                g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_ALERTING;
            }

            break;
        }
        case E_JRD_VOICE_CALL_STATE_CONVERSATION:
        {
            if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_ALERTING) {
                g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_CONVERSATION;
            }
            break;
        }
        case E_JRD_VOICE_CALL_STATE_DISCONNECTING:
        case E_JRD_VOICE_CALL_STATE_END:
        {
            #ifdef JRD_VOICE_DEBUG
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR, "<DISCONNECTING/END>Info, curr_status=%d, late_status=%d.\n", g_voice_data.curr_status, g_voice_data.late_status);
            #endif
            if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_CONVERSATION
                && (jan_voice_al_get_third_call_state () == E_JRD_VOICE_CALL_STATE_DISCONNECTING
                    || jan_voice_al_get_third_call_state () == E_JRD_VOICE_CALL_STATE_END))
            {
                jan_slic_set_tone_osc_time(g_voice_data.slic_dev, 0, 0);
                /* Clear g_third_call_info state. */
                jan_voice_al_clear_third_call_state ();
            }
            else if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_CONVERSATION)
            {
                /* For call end playing. */
                jan_slic_set_tone_osc_time(g_voice_data.slic_dev, 0, 0);        /* Close 3rd busy tone. */
                jan_slic_set_tone_osc_time (g_voice_data.slic_dev, 350, 350);
            }
            else
            {
                jan_slic_opt_pcm(g_voice_data.slic_dev, 1);                /* Enable PCM. */
                //jan_voice_al_call_end();                                     /* End Call. */

                /* For specific busy fixed line (such as [Latam] side). */
                jan_slic_set_tone_osc_time (g_voice_data.slic_dev, 350, 350);
            }
            
            /* Call In, when it's ringing disconnect or call end. */
            if (g_voice_data.curr_status == E_JRD_VOICE_CORE_STATUS_RING)
            {
                g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_IDLE;
            }
            break;
        }
        default:
        {
            JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Info, network state = %d.\n", call_info->call_state);
            break;
        }
    }

    /* Handle voice status. */
    if ((g_voice_data.curr_status != g_voice_data.late_status) && result != JRD_NO_ERR) {
        jan_voice_core_status_handle(g_voice_data.curr_status, g_voice_data.late_status);
        g_voice_data.curr_status = g_voice_data.late_status;
    }
    return JRD_NO_ERR;
}


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
void jan_voice_core_opt_ring(int opt)
{
    jan_slic_opt_ring(g_voice_data.slic_dev, opt);
}


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
int jan_voice_core_event_register(void *callback, void *cb_param)
{
    return jan_voice_al_event_register(callback, cb_param);
}


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
int jan_voice_core_event_unregister(void *callback)
{
    return jan_voice_al_event_unregister(callback);
}


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
int jan_voice_core_init(void)
{
    int rc = JRD_NO_ERR;

    g_voice_data.slic_dev = jan_voice_core_create_device();
    if (!g_voice_data.slic_dev) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, create slic device.\n");
        return JRD_FAIL;
    }

    rc = jan_voice_al_init();
    if (rc != JRD_NO_ERR) {
        JRD_OEM_LOG_INFO(JRD_OEM_LOG_ERROR,"Error, register to voice al.\n");
        return JRD_FAIL;
    }

    g_voice_data.curr_status = E_JRD_VOICE_CORE_STATUS_IDLE;
    g_voice_data.late_status = E_JRD_VOICE_CORE_STATUS_IDLE;

    /* Init Call-Out Timer. */
    g_voice_data.call_timer.ts.tv_sec = JRD_VOICE_CALL_WAIT_TIME; // 15s
    g_voice_data.call_timer.ts.tv_nsec = 0;
    g_voice_data.call_timer.cb.handler = jan_voice_core_wait_timeout;
    g_voice_data.call_timer.cb.params = g_voice_data.slic_dev;

    //sighandler_t callout_timeout_handler = jan_voice_core_timeout_busy_tone;
    //signal (SIGALRM, callout_timeout_handler);

    /* Init list. */
    //g_voice_data.id_count = 1;
    ordered_list_init(&g_voice_data.total, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_LTE);
    ordered_list_init(&g_voice_data.missed, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_LTE);
    ordered_list_init(&g_voice_data.incoming, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_LTE);
    ordered_list_init(&g_voice_data.outgoing, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_LTE);

    /* Init count info. */
    g_voice_data.count_info.max_count = 100;
    g_voice_data.count_info.incoming_count = 0;
    g_voice_data.count_info.outgoing_count = 0;
    g_voice_data.count_info.missed_count = 0;

    /* Init dtmf queue. */
    jan_dtmf_queue.head = 0;
    jan_dtmf_queue.tail = 0;

    /* Init call back function, that init data form datastore. */
    jan_voice_datastore_init(jan_voice_core_init_record_form_datastore);

    return JRD_NO_ERR;
}
