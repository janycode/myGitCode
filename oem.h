#ifndef _OEM_H_
#define _OEM_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdarg.h>
#include <dirent.h>
#include "string.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syscall.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <utils/queue.h>
#include "msg.h"
#include "msgcfg.h"
#include "comdef.h"
#include <utils/uemf.h>
#include <jsonc/json.h>
#include "common_def.h"
#include "db_common.h"
#include "timer.h"

#define INVALID_SYM_FD -1
#define ARRAY_SIZE(array, type) (sizeof(array)/sizeof(type))
#define TN_WEB_SYM_MAX 64
#define PARAM_INT (0)
#define PARAM_STR (1)
#define OEM_SUB_MASK_SIZE 4
#define OEM_THREAD_STACK_SIZE 64*1024
/*Begin weiqiang.lei add signals defined 2014-10-13*/
#define BIT(nr)                           (1 << (nr))

#define Q_CMD_SIGNAL                      (BIT(0))
#define SOFT_DOG_SIGNAL                   (BIT(1))
/*End weiqiang.lei add signals defined 2014-10-13*/

// Connie add start,2014/8/12
#define PARAM_ID(type_id_val)             ((type_id_val) & 0xFFFF)
#define PARAM_TYPE(type_id_val)           ((type_id_val) >> 16)
//Connie add end, 2014/8/12
#define OEM_MERGE_16BTO32B(h_16b, l_16b)  (((h_16b) << 16) | (l_16b))
#define OEM_8B_TO_16B(h_val, l_val)       ((h_val<<8)|(l_val))
#define OEM_MAIN_MASK_BIT_SET(val, bit)   (((val) & (1 << (bit))) ? 1 : 0)
#define OEM_SUB_MASK_BIT_SET(val, bit)    (val[(bit / 32)] & (1 << (bit % 32)))

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(arr)                       (sizeof(arr) / sizeof((arr)[0]))
#endif

typedef enum{
  WEBS_SET_SUCCESS         = 0x00,
  WEBS_SET_FAILED        = 0x01,
  WEBS_CALL_API_FAILED    = 0x02,
  WEBS_GET_UIPARA_WRONG   = 0x03,
} webs_asp_result_type;

typedef enum
{
   OEM_LOG_INVALID = -1,
   OEM_LOG_LOW,
   OEM_LOG_MEDIAM,
   OEM_LOG_HIGH,
   OEM_LOG_ERROR,
   OEM_LOG_FATAL,
   OEM_LOG_MAX,
}log_level_enum_type;

#define CORE_APP_LOG_SIZE 256

#define PRINT_MSG( level, fmtString, x)                         \
        MSG_SPRINTF_1(MSG_SSID_DIAG, level, fmtString, x)
int printf(char* sprint_buf, int size, char *fmt, ...);
int prinf_mem_info(void);

extern int core_app_log_level;
extern FILE *core_app_log_fp;

#define OEM_LOG_INFO(level, x...)                    \
  do{ \
    if(level >= core_app_log_level) \
    { \
        char sprint_buf[CORE_APP_LOG_SIZE]= {0}; \
        printf("%s(%d) ", __FUNCTION__, __LINE__); \
        printf(x); \
        printf(sprint_buf,CORE_APP_LOG_SIZE,x); \
        switch (level) \
        { \
          case OEM_LOG_LOW: \
            PRINT_MSG(MSG_LEGACY_HIGH, "%s", sprint_buf); \
          break; \
          case OEM_LOG_MEDIAM: \
            PRINT_MSG(MSG_LEGACY_HIGH, "%s", sprint_buf); \
          break; \
          case OEM_LOG_HIGH: \
            PRINT_MSG(MSG_LEGACY_HIGH, "%s", sprint_buf); \
          break; \
          case OEM_LOG_ERROR: \
            PRINT_MSG(MSG_LEGACY_ERROR, "%s", sprint_buf); \
          break; \
          case OEM_LOG_FATAL: \
            PRINT_MSG(MSG_LEGACY_FATAL, "%s", sprint_buf); \
          break; \
          default: \
            PRINT_MSG(MSG_LEGACY_MED, "%s", sprint_buf); \
          break; \
        }  \
        if(core_app_log_fp) \
        { \
          char str[128]; \
          snprintf(str,128,"%s(%d)",__FUNCTION__,__LINE__); \
          fputs(str,core_app_log_fp); \
          fputs(sprint_buf,core_app_log_fp); \
          fflush(core_app_log_fp); \
        } \
    } \
  }while(0)

#define LOG_ERROR(level,x...) \
do { \
        if(level >= core_app_log_level) \
        { \
          printf("%s(%d) ", __FUNCTION__, __LINE__); \
          printf(x); \
        } \
} while(0)

#define MALLOC(sz,ptr) \
do{  \
    (ptr)=malloc((sz));  \
    if(NULL!=(ptr))  \
    memset((ptr),0,(sz)); \
}while(0)

#define MEMCPY(dest, src, sz) \
do{  \
    if((dest) && (src) && (sz)) \
      memcpy((dest), (src), (sz)); \
}while(0)

#define MEMSET(dest, ch, sz) \
do{  \
    if((dest) && (sz)) \
      memset((dest), (ch), (sz)); \
}while(0)

#define FREE(ptr) \
do{  \
    if(NULL!=(ptr)) \
      free((ptr));  \
    (ptr) = NULL; \
}while(0)

#define REMALLOC(sz,sz_new,ptr) \
do{ \
  if(sz_new > sz) \
  { \
    void *re_ptr = NULL; \
    (re_ptr)=malloc((sz_new)); \
    if(NULL!=(re_ptr))  \
    { \
      memset((re_ptr),0,(sz_new)); \
      if(NULL!=(ptr)) \
      { \
       memcpy((re_ptr), (ptr), (sz)); \
      } \
    } \
    if(NULL!=(ptr)) \
    { \
       free(ptr); \
    } \
    (ptr) = (re_ptr); \
  } \
}while(0)

#define STRLEN(src) strlen(src)

#define STRCMP(dest, src) strcmp((dest), (src))

#define STRNCMP(dest, src, n)    strncmp((dest), (src), (n))

#define STRCAT(dest, src)    strcat((dest), (src))

#define STRCPY(dest, src) \
  do{  \
    if((dest) && (src)) \
      strcpy((dest), (src)); \
  }while(0)

#define STRNCPY(dest, src, n) \
do{ \
  if(NULL == (src)) \
  {\
    (dest)[0] = 0; \
  } \
  strncpy((dest), (src), (n)); \
}while(0)

#define STRTOK_R(str, delim, saveptr) strtok_r(str, delim, saveptr)



typedef FILE file_t;
typedef DIR dir_t;
typedef struct dirent dirent_t;
typedef struct stat file_stat_t;

#define OPEN(path, mode)                      open(path, mode)
#define READ(fd, buf, size)                   read(fd, buf, size)
#define CLOSE(fd)                             close(fd)
#define FOPEN(path,  mode)                    fopen(path, mode)
#define FCLOSE(file)                          fclose(file)
#define FSEEK(fp, offset, whence)             fseek(fp, offset, whence)
#define FTELL(fp)                             ftell(fp)
#define FREAD(buf, size, offset, fp)          fread(buf, size, offset, fp)
#define WRITE(buf, size, offset, fp)          fwrite(buf, size, offset, fp)
#define FPRINTF(stream, format...)            fprintf(stream, format)
#define ACCESS(file, mode)                    access(file, mode)
#define STAT(file, state)                     stat(file, state)
#define S_ISDIR(st_mode)                      S_ISDIR(st_mode)
#define REMOVE(path)                          remove(path)
#define MKDIR(dir, mode)                      mkdir(dir, mode)
#define OPENDIR(dir)                          opendir(dir)
#define READDIR(dir)                          readdir(dir)
#define CLOSEDIR(dir)                         closedir(dir)

#define SNPRINTF(str, size, format...)        snprintf(str, size, format)
#define SPRINTF(str, format...)               sprintf(str, format)
#define SSCANF(str, format...)                sscanf(str, format)
#define INTDUP(i)                             intdup(i)
#define STRDUP(s)                             strdup(s)

#define ATOI(str)                             atoi(str)
#define POPEN(command, type)                  popen(command, type)
#define PCLOSE(stream)                        pclose(stream)
#define CALLOC(n, size)                       calloc(n, size)
#define REWIND(f)                             rewind(f)
#define SYNC()                                sync()
#define UMASK(cmask)                          umask(cmask)
#define FSCANF(str, format...)                fscanf(str, format)
#define FILENO(f)                             fileno(f)
#define FSTAT(fd, stat)                       fstat(fd, stat)
#define FGETS(buf, bufsize, stream)           fgets(buf, bufsize, stream)
#define SLEEP(n)                              sleep(n)
#define FFLUSH(stream)                        fflush(stream)
#define FSYNC(fd)                              fsync(fd)

// Connie add start, 2014/8/11
#define CRASH() \
  do \
  { \
    char *NULL_PTR = NULL; \
    OEM_LOG_INFO(OEM_LOG_ERROR, "need crash here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); \
    memcpy(NULL_PTR,"crash",strlen("crash")); \
  }while(0)

// Connie add end, 2014/8/11

static inline int * intdup(int val)
{
  int *p_val = NULL;
  MALLOC(sizeof(int), p_val);
  if (p_val!=NULL)
  {
    *p_val = val;
  }
  return p_val;
}


typedef struct {
    uint32 sig_set;
    uint32 timed_out;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} os_signal_type;

#define OS_SIGNAL_INIT(ptr) \
  do { \
    (ptr)->sig_set = 0; \
    (ptr)->timed_out = 0; \
    pthread_cond_init(&(ptr)->cond, NULL); \
    pthread_mutex_init(&(ptr)->mutex, NULL); \
  } while(0)

#define OS_SIGNAL_DESTROY(ptr) \
do{ \
    pthread_cond_destroy(&(ptr)->cond); \
    pthread_mutex_destroy(&(ptr)->mutex); \
}while(0)

#define OS_SIGNAL_DEINIT(ptr) \
  do {  \
    (ptr)->sig_set = 0; \
    (ptr)->timed_out = 0; \
    pthread_cond_destroy(&(ptr)->cond); \
    pthread_mutex_destroy(&(ptr)->mutex); \
  } while(0)

#define OS_SIGNAL_WAIT(ptr)  \
  do { \
    LOCK(ptr);\
    pthread_cond_wait(&(ptr)->cond, &(ptr)->mutex); \
    UNLOCK(ptr);; \
  } while(0)

#define OS_SIGNAL_SET(ptr)  \
  do { \
    LOCK(ptr);\
    (ptr)->sig_set = 1; \
    pthread_cond_signal(&(ptr)->cond); \
    UNLOCK(ptr);\
  } while(0)

#define OS_SIGNAL_CLR(ptr, sig)  \
  do { \
    (ptr)->sig_set &= ~sig; \
  } while(0)

#define OS_SIGNAL_GET(ptr, sig)  \
  do { \
    sig = (ptr)->sig_set; \
  } while(0)
#define LOCK(ptr) \
do { \
     pthread_mutex_lock((&(ptr)->mutex)); \
}while(0)
#define UNLOCK(ptr)  \
do { \
     pthread_mutex_unlock((&(ptr)->mutex)); \
}while(0)

#define LOCK_EX(ptr) \
do { \
     if(NULL!=(ptr))  \
     pthread_mutex_lock(ptr); \
}while(0)
#define UNLOCK_EX(ptr)  \
do { \
     pthread_mutex_unlock(ptr); \
}while(0)

#define MUTEX_INIT_EX(ptr) \
  do { \
    if(NULL!=(ptr))  \
    pthread_mutex_init((ptr), NULL); \
  } while(0)

#define OS_SIGNAL_EX(ptr)  \
  do { \
    pthread_cond_signal(ptr); \
  } while(0)

#define OEM_SET_VALUE(param, value, size, mutex) \
  do { \
    LOCK_EX(mutex); \
    MEMCPY(param, value, size); \
    UNLOCK_EX(mutex); \
  }while(0)

/*static inline uint32 os_signal_handle(os_signal_type *ptr)
{
    uint32 req_signal = 0;

    OS_SIGNAL_WAIT(ptr);
    OS_SIGNAL_GET(ptr, req_signal);
    OS_SIGNAL_CLR(ptr, req_signal);

    return req_signal;
}*/
#define os_signal_handle(ptr) \
({ \
  uint32 req_signal = 0; \
  OS_SIGNAL_WAIT(ptr); \
  OS_SIGNAL_GET(ptr, req_signal); \
  OS_SIGNAL_CLR(ptr, req_signal); \
  req_signal;\
})

extern pid_t gettid(void);

#define print_thread_pid(void) \
do{ \
  char print_cmd[256] = {0}; \
  SNPRINTF(print_cmd,sizeof(print_cmd),"echo \"%s pid:%d\" >> /core_app_thread_pid",__FUNCTION__,gettid()); \
  system(print_cmd); \
}while(0)


static void get_socket_path(char *outBuffer)
{
    FILE *f;
    char buf[64] = {0};
    const char* socket_path = "/dev/socket/qmux_webs/server_webs";
    const char* socket_path_file = "/usr/oem/socket_addr";
    int t_len = 0;
    f = FOPEN(socket_path_file, "r");
    if(f == NULL)
    {
        STRCPY(buf, socket_path);
    t_len = STRLEN(buf);
    }
     else
    {
        FSEEK(f, 0L, SEEK_END);
        t_len = FTELL(f);
        REWIND(f);
        FREAD(buf, 1, t_len , f);
        FCLOSE(f);
    t_len = STRLEN(buf) - 1;
    }

  MEMCPY(outBuffer,buf,t_len);
  OEM_LOG_INFO(OEM_LOG_LOW, "socket address:%s\n", buf);
  //OEM_LOG_INFO(OEM_LOG_ERROR, "socket address:%s\n", buf);
}


extern int
wait_for_sig_with_timeout
(
  os_signal_type  *signal_ptr,
  uint32                         timeout_milli_secs
);

#define WAIT_FOR_SIGNAL(signal_ptr, timeout_secs) \
  wait_for_sig_with_timeout (signal_ptr,timeout_secs)




typedef enum
{
   CMD_INVALID = -1,
   CMD_WEB_REQ,
   CMD_RSP_WEB,
   CMD_UI_REQ,
   CMD_RSP_UI,
   CMD_NORMAL_REQ,
   CMD_RSP_NORMAL,
   CMD_IND,
   CMD_MAX,
}cmd_enum_type;

typedef enum
{
   MODULE_INVALID = -1,
   MODULE_MIN,
   MODULE_MAIN = MODULE_MIN,
   MODULE_SOCK,
   MODULE_SMS,
   MODULE_USIM,
   MODULE_CONNECTION,
   MODULE_PROFILE,                 /*5*/
   MODULE_NETWORK,
   MODULE_WIFI,
   MODULE_ROUTER,
   MODULE_SYS,
   MODULE_MISC,                    /*10*/
   MODULE_SDSHARE,
   MODULE_LCD,
   MODULE_DEBUG,
   MODULE_PBM,
   MODULE_CHG,                     /*15*/
   MODULE_UI,
   MODULE_KEY,
   MODULE_TIME,
   MODULE_INPUT_LISTEN,
   MODULE_LED,                     /*20*/
   MODULE_FOTA,
   MODULE_USAGE,
   MODULE_SOFT_DOG,
   MODULE_VOICE,                       //Add by ck:add tel module.can be optimized by #define
 //Added by Tian Yiqing, Add cwmp module, Start.
#ifdef FEATURE_CWMP
   MODULE_CWMP,
#endif
 //Added by Tian Yiqing, Add cwmp module, End.
   MODULE_MAX,
}module_enum_type;

/*Begin weiqiang.lei add thread start flag from file defined 2014-10-14*/
typedef struct soft_dog
{
    uint32    unkickdog_count;        /*How many times did not the recording thread action in response to kick the dog*/
    boolean  kickdog_flag;              /*current kick dog flag*/
    boolean  softdog_enable;         /*Enable SoftDog listening identify*/
}softdog_record;

typedef struct
{
    module_enum_type   id;     /*thread id*/
    char         *name;          /*thread name*/
    boolean     disable_flag;   /*thread Start marking*/
    uint32       dog_count;     /*Timeout thread count times SoftDog*/
    softdog_record softdog; /*softdog struct*/
}thread_base_type;
/*End weiqiang.lei add thread start flag from file defined 2014-10-14*/

#define INVALID_ACT_ID (-1)
#define INVALID_IND_ID (-1)

typedef void (*fp_cmd_cb_type)
(
  void *cb_data, void *user_data
);

typedef q_type* (*fp_get_queue_cb_type)
(
  uint32 act_id
);


typedef struct{
  cmd_enum_type type;
  module_enum_type source_m_id;
  module_enum_type dest_m_id;
  uint32    act_id;
  boolean       is_sync;
}cmd_hdr_type;

typedef struct
{
  uint32 sub_mask[OEM_SUB_MASK_SIZE];
// Connie add start for json parser, 2014/8/12
  json_object * object_req;
  json_object * object_rsp;
// Connie add end for json parser, 2014/8/12
  uint8 *req_buf;
  uint32 req_buf_len;
  uint8 *rsp_buf;
  uint32 rsp_buf_len;
  int error_code;
  fp_cmd_cb_type     cmd_cb;
  void   *user_data;
}cmd_data_type;

typedef struct{
  module_enum_type module_id;
  os_signal_type* p_os_sig;
  q_type*  p_module_q;
  fp_get_queue_cb_type q_get_cb;
  pthread_mutex_t *ex_mutex;
}thread_info_type;

typedef void* (*func_vp_vp)(void* args);
typedef struct oem_thread_data {
    pthread_t thread_handle;
    os_signal_type thread_signal;
    q_type thread_queue;
     size_t thread_stacksize;
    func_vp_vp thread_func;
    void *thread_user_data;
} oem_thread_data_t;

typedef struct {
 module_enum_type module_id;
 uint32                     ind_id;
 void*                      ind_data;
 uint32                     data_size;
} ind_data_type;

typedef int (*cmd_func_t)(cmd_data_type *params);

typedef struct {
 q_link_type                 link;
 cmd_hdr_type  cmd_hdr;
 void     *cmd_data;
} cmd_q_type;

typedef struct{
  uint32 param_type_id;
  char* param_key;
}web_param_info_type;

typedef struct{
  module_enum_type module_id;
  uint16           act_id;
  uint32           sub_mask[OEM_SUB_MASK_SIZE];
}web_act_info_type;

typedef struct{
  char* tn_web_req_name;
  web_act_info_type* module_act_info;
}web_data_info_type;

typedef struct{
  sym_fd_t sym_fd;
  web_param_info_type*     web_param_info;
}tn_web_param_info_type;

// Connie add start, 2014/12/22
typedef struct{
  int local_error_code;
  int rsp_error_code;
  char* err_code_str;
  char* err_message;
}module_error_code_mapping_table_type;

typedef struct{
  module_enum_type                      module_id;
  module_error_code_mapping_table_type* module_error_code_table;
}error_code_summary_type;
// Connie add end, 2014/12/22

typedef struct{
  uint32         req_total_num;
  uint32         req_mask;
  q_type         req_rsp_q;
  sock_hdr_t sock_hdr;
  int            fd;
  msg_head_t     msg_head;
  int            rsp_total_param;
  int            rsp_buf_total_len;
  char           id[10]; // Connie add for json parser, 2014/8/12
  json_object   *object_web_data;  // Connie add, 2014/9/23
} oem_web_data_type;

typedef struct{
  q_link_type link;
  uint32 req_num;
  oem_web_data_type *oem_web_data;
  cmd_q_type     *cmd_q_data;
}web_data_q_type;

// Connie add start, 2014/12/26
typedef struct {
  int    local_enum_value;
  struct {
          char *str;
          int  val;
        } web_value;
} enum_mapping_info_type;

typedef struct {
  uint16       param_id;
  uint8        param_type;  /* 0: INT; 1: STR */
  enum_mapping_info_type *enum_mapping_table;
} param_mapping_info_type;

typedef struct {
  uint16       module_id;
  param_mapping_info_type *param_mapping_table;
} module_mapping_info_type;
// Connie add end, 2014/12/26

// Connie add start, 2014/12/27
typedef struct {
  uint16     param_id;
  //boolean    need_free_param_data;
  void      *param_data;// maybe json_object_info_type* or json_array_info_type*
}json_name_value_pairs_info_type;

typedef struct {
  int                               NameValuePairs_count;
  json_name_value_pairs_info_type  *NameValuePairs_data;
}json_object_info_type;

typedef struct {
  int     member_count;
  uint16  member_type;
  void   *member_data; //maybe char** or json_object_info_type*
}json_array_info_type;
// Connie add end, 2014/12/27

typedef struct system_call_status_struct
{
  boolean if_shell_finished;
  int exit_code;//shell exit code. WARNING!!!if shell not exist or no permission, "if_shell_finished" is true, code is 126 or 127.
  boolean if_finished_by_signal;//for example, kill the shell it will be 1
  int signal_code;
  boolean if_stoped;//if paused, use WUNTRACED can cause this
  int stop_signal_code;
}system_call_error_code_t;

typedef int (*oem_act_func_type)(cmd_data_type *cmd_data);


#ifdef __cplusplus
}
#endif

#endif


