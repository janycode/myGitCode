#ifndef __DB_USAGE_SQLITE3_H__
#define __DB_USAGE_SQLITE3_H__

#include <time.h>
typedef  unsigned long long  uint64;

typedef struct {
	time_t start_time;
	time_t end_time;
} usage_history_time_dur_type;

typedef struct {
  time_t            start_time;
  time_t            end_time;
  int               roam_type;
  uint64            tx_bytes;
  uint64            rx_bytes;
} usage_history_type;

typedef struct
{
    usage_history_type *pRecordDetail;
    int  nRecordLenRet;
    int  nRecordLen;
} TUsageHistoryRecordListInter;

typedef enum
{
   E_LIMIT_MSG = 0,
   E_MONTHLY_MSG,
}e_DevicePushInfoMsg_type;

typedef struct DevicePushInfo{
	char deviceToken[128];
	char macAddress[20];
	char osType[20];
	char message[256];
	char monthlymessage[256];
	char pushTime[20];
	char monthlypushTime[20];
}SDevicePushInfo;

typedef struct DevicePushInfoListReq{
	SDevicePushInfo *pInfo;
	int length;
	int realLen;
}SDevicePushInfoListReq;

typedef struct DevicePushInfoList{
	SDevicePushInfo *pInfo;
	int length;
}SDevicePushInfoList;



//insert
int db_set_dev(const SDevicePushInfo *pInfo);
//find
int db_get_dev_lst(SDevicePushInfoList *pList);
int db_get_dev(const char deviceToken[128], SDevicePushInfo *pInfo);
//update
int db_update_push_time(const char deviceToken[128], e_DevicePushInfoMsg_type message_type);
int db_clear_push_time(e_DevicePushInfoMsg_type message_type);
//delete
int db_del_dev_by_mac(char mac_address[20]);
int db_del_dev_by_token(const char deviceToken[128]);

//insert
extern int db_insert_usage_history_data_internal(const usage_history_type *usage);
//find
int db_get_usage_total_num_data(const usage_history_time_dur_type *interval, TUsageHistoryRecordListInter *usageListRecord);
//update
extern int db_update_usage_history_data_internal(time_t old_end_time, const usage_history_type *usage);
//delete
extern int db_del_usage_history_record(time_t date);
#endif

