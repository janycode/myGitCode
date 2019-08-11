/***************************************************************************
* Description: 
*
**********************************History***********************************
*     when              who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  2017/02/23
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/types.h>

#include "jan_oem.h"
#include "jan_voice_soundcard.h"

#define JRD_VOICE_SOUNDCARD_PATH "/proc/asound/mdm9607tomtomi2"

#define JRD_VOICE_OPEN_CSVOICE_RX_PATH "amix 'AUX_PCM_RX_Voice Mixer CSVoice' 1"
#define JRD_VOICE_CLOSE_CSVOICE_RX_PATH "amix 'AUX_PCM_RX_Voice Mixer CSVoice' 0"
#define JRD_VOICE_OPEN_CSVOICE_TX_PATH "amix 'Voice_Tx Mixer AUX_PCM_TX_Voice' 1"
#define JRD_VOICE_CLOSE_CSVOICE_TX_PATH "amix 'Voice_Tx Mixer AUX_PCM_TX_Voice' 0"
#define JRD_VOICE_CSVOICE_PCM_NODE_PLAY "aplay -D hw:0,2 -P &"
#define JRD_VOICE_CSVOICE_PCM_NODE_REC "arec -D hw:0,2 -P -R 8000 -C 1 &"

#define JRD_VOICE_OPEN_VOLTE_RX_PATH "amix 'AUX_PCM_RX_Voice Mixer VoLTE' 1"
#define JRD_VOICE_CLOSE_VOLTE_RX_PATH "amix 'AUX_PCM_RX_Voice Mixer VoLTE' 0"
#define JRD_VOICE_CLOSE_VOLTE_TX_PATH "amix 'VoLTE_Tx Mixer AUX_PCM_TX_VoLTE' 0"
#define JRD_VOICE_OPEN_VOLTE_TX_PATH "amix 'VoLTE_Tx Mixer AUX_PCM_TX_VoLTE' 1"
#define JRD_VOICE_VOLTE_PCM_NODE_PLAY "aplay -D hw:0,4 -P &"
#define JRD_VOICE_VOLTE_PCM_NODE_REC "arec -D hw:0,4 -P -R 8000 -C 1 &"

/*===========================================================================
  Function:  jan_voice_soundcard_exist
===========================================================================*/
/*!
@brief
  Detected Sound-Card exist.

@parameters
  None.

@return
  None.
*/
/*=========================================================================*/
static int jan_voice_soundcard_exist(void)
{
    int count = 0;

    do {
        if (access(JRD_VOICE_SOUNDCARD_PATH, F_OK) == 0) {
            break;
        } else {
            printf("%s(): Error, soundcard not exist(count = %d), path is: %s\n",
                __func__, count, JRD_VOICE_SOUNDCARD_PATH);

            //sleep(10);  //Delay 10s
            sleep (5);
            count++;
        }
    } while (count < 3);

    if (count < 3) {
        return 0;
    } else {
        return -1;
    }
}


/*===========================================================================
  Function:  jan_voice_soundcard_pcm_output
===========================================================================*/
/*!
@brief
  Output PCM.

@parameters
  None.

@return
  None.
*/
/*=========================================================================*/
static void jan_voice_soundcard_pcm_output(void)
{
    char command[128] = {0};
    FILE *fp = NULL;

    /* Open Voice Channel. */
    //system("amix 'AUX_PCM_RX_Voice Mixer CSVoice' 1");
    //system("amix 'Voice_Tx Mixer AUX_PCM_TX_Voice' 1");
    system(JRD_VOICE_OPEN_CSVOICE_RX_PATH);
    system(JRD_VOICE_OPEN_CSVOICE_TX_PATH);
    system(JRD_VOICE_OPEN_VOLTE_RX_PATH);
    system(JRD_VOICE_OPEN_VOLTE_TX_PATH);

    JRD_MEMSET(command, 0, sizeof(command));
    JRD_SNPRINTF(command, sizeof(command), JRD_VOICE_CSVOICE_PCM_NODE_PLAY);
    fp = JRD_POPEN(command, "r");
    if (NULL == fp) {
        printf("%s(): Error, popen %s failed.\n", __func__, command);
        return;
    }
    JRD_PCLOSE(fp);
    fp = NULL;

    JRD_MEMSET(command, 0, sizeof(command));
    JRD_SNPRINTF(command, sizeof(command), JRD_VOICE_CSVOICE_PCM_NODE_REC);
    fp = JRD_POPEN(command, "r");
    if (NULL == fp) {
        printf("%s(): Error, popen %s failed.\n", __func__, command);
        return;
    }
    JRD_PCLOSE(fp);
    fp = NULL;
}


/*===========================================================================
  Function:  jan_voice_soundcard_aplay_detect
===========================================================================*/
/*!
@brief
  Detected process "aplay -D hw:0,2 -P" isn't exsit.

@parameters
  None.

@return
  None.
*/
/*=========================================================================*/
static int jan_voice_soundcard_process_detect(char *process_name)
{
    FILE *f_ptr = NULL;
    char *pid_ptr = NULL;
    char command[128] = {0};
    char ps_buf[512] = {0};
    int pid_num = 0;
    pid_t detected_pid;
    int rc = 0;

    if (process_name == NULL) {
        printf("%s(): Error, process name is NULL.\n", __func__);
        return -1;
    }

    sprintf(command, "ps -ef | grep %s", process_name);
    f_ptr = popen(command, "r");
    if (f_ptr == NULL) {
        printf("%s(): Error, failed to popen: %s.\n", __func__, command);
        return -1;
    }

    fgets(ps_buf, sizeof(ps_buf), f_ptr);
#ifdef VOICE_DEBUG
    printf("%s(): Info, ps_buf: %s", __func__, ps_buf);
#endif

    pclose(f_ptr);
    pid_ptr = strtok(ps_buf, " ");

    if ((pid_ptr == NULL)
        || (strlen(pid_ptr) == 0)
        || ((pid_num = atoi(pid_ptr)) == 0)) {
        printf("%s(): Error, failed to get pid number: %s.\n", __func__, pid_ptr);
        return -1;
    }

#ifdef VOICE_DEBUG
    printf("%s(): Info, pid_num: %d.\n", __func__, pid_num);
#endif
    detected_pid = (pid_t)pid_num;
    rc = kill(detected_pid, 0);
    if (rc != 0) {
        printf("%s(): Info, process: %s not exist.\n", __func__, process_name);
        return 1;
    }

    printf("%s(): Info, process: %s is exist.\n", __func__, process_name);
    return 0;
}


/*===========================================================================
  Function:  jan_voice_soundcard_init
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
int jan_voice_soundcard_init(void)
{
    int rc = 0;

    rc = jan_voice_soundcard_exist();
    if (rc < 0) {
        printf("%s(): Error, soundcard cann't init.\n", __func__);
        return -1;
    }

    /* Config, PCM waveform output. */
    rc = jan_voice_soundcard_process_detect("aplay");
    if (rc == 1) {
        /* Process: "aplay -D hw:0,2 -P" not exist. */
        jan_voice_soundcard_pcm_output();
    } else if (rc == 0) {
        /* Process: "aplay -D hw:0,2 -P" is exist. */
    } else {
        printf("%s(): Error, detect process aplay.\n", __func__);
        return -1;
    }
    return 0;
}
