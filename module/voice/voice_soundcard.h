/***************************************************************************
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
  Function:  jan_slic_soundcard_init
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
int jan_slic_soundcard_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _JRD_VOICE_SOUNDCARD_H_ */
