/******************************************************************************
  @file    time.h
  @brief   The time utility functions used in various places in core-app.

  DESCRIPTION
  Time common utility functions

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2015 jan Incorporated.
  All Rights Reserved. jan Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/


/******************************************************************************


                        EDIT HISTORY FOR MODULE

when       who      			what, where, why
--------   ---      ---------------------------------------------------------- 
2013.11.20   li.kuang             create

******************************************************************************/
#ifndef _h_TIME
#define _h_TIME 1

/*******************************  Includes ************************************/
#include "comdef.h"
#include "wmsts.h"

void change_time_to_string_for_EE(char* des_time,uint32 time_span);
void change_time_to_string_for_usa(char* des_time,uint32 time_span);
void change_time_to_string_for_general(char* des_time,uint32 time_span);
uint32 change_time_to_sms_span(char * time_str);
void   jan_qmi_timestamp_to_secs( wms_timestamp_s_type  time, uint32 *time_stick);
void   jan_qmi_secs_to_timestamp( uint32 time_stick, wms_timestamp_s_type  *time);
#endif /* _h_TIME */

/******************************************************************************/

