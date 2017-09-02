/***************************************************************************
*     Copyright (c) 2016 by Fan.Xi.  All Rights Reserved.
****************************************************************************
* File:  jrd_voice_datastore.h
* Author:  Fan Xi <xifan2012@gmail.com>
* Description: 
*
**********************************History***********************************
*     when              who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  2017/04/27
*
****************************************************************************/

#ifndef _JRD_VOICE_DATASTORE_H_
#define _JRD_VOICE_DATASTORE_H_


typedef void (*jrd_voice_datastore_cb)(int item_count, char** item_value, char** item_name);


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
  Function:  jrd_voice_datastore_insert
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
extern int jrd_voice_datastore_insert(jrd_voice_call_record_t *record, int limit_num);


/*===========================================================================
  Function:  jrd_voice_datastore_limit
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
int jrd_voice_datastore_limit(jrd_voice_call_record_t *record, int limit_num);


/*===========================================================================
  Function:  jrd_voice_datastore_delete
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
extern int jrd_voice_datastore_delete(int id);


/*===========================================================================
  Function:  jrd_voice_datastore_clear
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
extern int jrd_voice_datastore_clear(void);


/*===========================================================================
  Function:  jrd_voice_datastore_init
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
extern int jrd_voice_datastore_init(jrd_voice_datastore_cb ptr_cb);


#ifdef __cplusplus
}
#endif

#endif /* _JRD_VOICE_DATASTORE_H_ */