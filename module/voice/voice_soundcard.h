/***************************************************************************
*     Copyright (c) 2016 by Fan.Xi.  All Rights Reserved.
****************************************************************************
* File:  jrd_voice_soundcard.h
* Author:  Fan Xi <xifan2012@gmail.com>
* Description: 
*
**********************************History***********************************
*     when              who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  2017/02/23
*
****************************************************************************/

#ifndef _JRD_VOICE_SOUNDCARD_H_
#define _JRD_VOICE_SOUNDCARD_H_


#define JRD_VOICE_SOUNDCARD_CSVOICE_DEVICE  "hw:0,2"
#define JRD_VOICE_SOUNDCARD_VOLTE_DEVICE  "hw:0,4"


#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
  Function:  jrd_slic_soundcard_init
===========================================================================*/
/*!
@brief
  Init Sound-Card and Output PCM.

@parameters
  None.

@return
  None.
*/
/*=========================================================================*/
int jrd_slic_soundcard_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _JRD_VOICE_SOUNDCARD_H_ */
