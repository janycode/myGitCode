#ifndef _DB_H_
#define _DB_H_

#include <stdio.h>
#include "comdef.h"
#include <diag/msg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NO_ERROR 0
#define FAIL     1

typedef enum{
  E_LOADER_USER = 0,
  E_LOADER_FACTORY,
  E_LOADER_RESTORE,
  E_LOADER_MAX,
}e_loder_sqlite;

#define FREE(ptr) \
do{  \
      if(NULL!=(ptr)) \
        free((ptr));  \
      (ptr) = NULL; \
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

#define STRNCPY(dest, src, n) \
do{ \
	if(NULL == (src)) \
	{\
		(dest)[0] = 0; \
	} \
	strncpy((dest), (src), (n)); \
}while(0)


#define OEM_LOG_LOW     MSG_LEGACY_HIGH   /*4*/
#define OEM_LOG_MEDIAM  MSG_LEGACY_ERROR  /*8*/
#define OEM_LOG_HIGH    MSG_LEGACY_ERROR  /*8*/
#define OEM_LOG_ERROR   MSG_LEGACY_FATAL  /*16*/

#define PRINT_MSG( level, fmtString, x)                         \
        MSG_SPRINTF_1(MSG_SSID_DIAG, level, fmtString, x)

#define SOCK_LOG_SIZE 256

#define OEM_LOG_INFO(level, x...)                    \
  do{ \
    if(level >= OEM_LOG_LOW) \
    { \
        char sprint_buf[SOCK_LOG_SIZE]; \
        memset(sprint_buf, 0, SOCK_LOG_SIZE); \
        printf("%s(%d) ", __FUNCTION__, __LINE__); \
        printf(x); \
        strcpy(sprint_buf, "sqlite3-db: "); \
        printf(sprint_buf+13, SOCK_LOG_SIZE-13, x); \
        PRINT_MSG(level, "%s", sprint_buf); \
    } \
  }while(0)

int printf(char* sprint_buf, int size, char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

