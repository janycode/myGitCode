/*
 * DevicePushInfo.c
 *
 *  Created on: Jun 4, 2013
 *      Author: fspi
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "wlan_event_listen.h"
#include <time.h>
//#include "usage_time_utility.h"
#include <sys/wait.h>
#include "db_usage_sqlite3.h"
#include "db.h"
//#include "usage_ios_push.h"
//#include "usage.h"
//#include "wlan_cfg.h"


#ifndef SQLITE_COMMAND_LENGTH_MAX
#define SQLITE_COMMAND_LENGTH_MAX   256
#endif

#if 0
typedef struct DevicePushInfoListReq{
	SDevicePushInfo *pInfo;
	int length;
	int realLen;
}SDevicePushInfoListReq;
#endif


static int EncodeEscapeString(const char *inStr, char *outbuf, int outbuf_len)
{
  int inStr_len;
  int i,j = 0;

  if (!inStr || !outbuf || outbuf_len == 0)
  {
		OEM_LOG_INFO(OEM_LOG_ERROR,"Pointer is NULL!\n");
		return -1;
  }

  memset(outbuf, 0, outbuf_len);
  inStr_len = strlen(inStr);

  for(i=0; i<inStr_len; i++)
  {
    if (j >= (outbuf_len-1))
      break;

    switch(inStr[i])
    {
      case '\'': //' -> #39$&
      {
        if ((j+5) < outbuf_len)
        {
          memcpy(&outbuf[j], "#39$&", 5);
        }
        j+=5;
        break;
      }

      default:
      {
        outbuf[j++] = inStr[i];
        break;
      }
    }
  }
  return 0;
}

static int DecodeEscapeString(char *inStr, char *outbuf, int outbuf_len)
{
  int inStr_len;
  int i = 0,j = 0;

  if (!inStr || !outbuf || outbuf_len == 0)
  {
		OEM_LOG_INFO(OEM_LOG_ERROR,"Pointer is NULL!\n");
		return -1;
  }

  memset(outbuf, 0, outbuf_len);
  inStr_len = strlen(inStr);

  while(i<inStr_len && j < (outbuf_len-1))
  {
    switch(inStr[i])
    {
      case '#': //#39$& -> '
      {
        char esc[6]= {0};
        memcpy(esc, &inStr[i], 5);
			  if (strcmp(esc, "#39$&") == 0)
			  {
          outbuf[j++] = '\'';
          i+=5;
        }
        else
        {
          outbuf[j++] = inStr[i++];
        }
        break;
      }

      default:
      {
        outbuf[j++] = inStr[i++];
        break;
      }
    }
  }
  return 0;
}

int FindDevicePushInfoCallback(void *param, int n_columm, char **column_value, char **column_name)
{
	int nRet = NO_ERROR;
	boolean *pBlFound = (boolean *)param;
	OEM_LOG_INFO(OEM_LOG_LOW,"SetUsageHistoryDataCallBack\n");

	*pBlFound = TRUE;
	return nRet;
}

static int __InsertDevicePushInfo(const SDevicePushInfo *pInfo)
{
	int nRet = NO_ERROR;
	char sqlCmd[SQLITE_COMMAND_LENGTH_MAX];
	char escape_message[256];
	char escape_message2[256];

	EncodeEscapeString(pInfo->message, escape_message, 256);
	EncodeEscapeString(pInfo->monthlymessage, escape_message2, 256);

	sprintf(sqlCmd, "INSERT INTO DevicePushInfo(DeviceToken, MacAddress, OSType, Message, MonthlyMessage) VALUES('%s', '%s', '%s', '%s', '%s')",
			pInfo->deviceToken, pInfo->macAddress, pInfo->osType, escape_message, escape_message2);

	nRet = db_exec_cmd(sqlCmd, NULL, NULL,E_LOADER_USER);

	if(NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"SetDevicePushInfo execute error:%d ", nRet);
	}
	return nRet;

}

static int __UpdateDevicePushInfo(const SDevicePushInfo *pInfo)
{
	int nRet = NO_ERROR;
	char sqlCmd[SQLITE_COMMAND_LENGTH_MAX];
	char escape_message[256];
	char escape_message2[256];
	EncodeEscapeString(pInfo->message, escape_message, 256);
	EncodeEscapeString(pInfo->monthlymessage, escape_message2, 256);

	sprintf(sqlCmd, "UPDATE DevicePushInfo SET MacAddress='%s', OSType='%s', Message='%s', MonthlyMessage='%s' WHERE DeviceToken='%s'",
			pInfo->macAddress, pInfo->osType, escape_message, escape_message2, pInfo->deviceToken);

	OEM_LOG_INFO(OEM_LOG_LOW,"SetDevicePushInfo select command: %s\n", sqlCmd);

	nRet = db_exec_cmd(sqlCmd, NULL, NULL,E_LOADER_USER);
	if(NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"SetDevicePushInfo execute error:%d ", nRet);
	}

	return nRet;
}

//int SetDevicePushInfoInter(const SDevicePushInfo *pInfo)
int db_set_dev(const SDevicePushInfo *pInfo)
{
	int nRet = NO_ERROR;
	if (pInfo == NULL)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"SetDevicePushInfo pInfo is NULL!\n");
		return nRet;
	}

	boolean blFound = FALSE;
	char sqlCmd[SQLITE_COMMAND_LENGTH_MAX];

	sprintf(sqlCmd, "SELECT * FROM DevicePushInfo WHERE DeviceToken='%s'", pInfo->deviceToken);
	OEM_LOG_INFO(OEM_LOG_LOW,"SetDevicePushInfo select command: %s\n", sqlCmd);


	//nRet = ExecuteCommand(pDeviceInfoDB, sqlCmd, FindDevicePushInfoCallback, (void*)&blFound, pErrMsg);
	nRet = db_exec_cmd(sqlCmd, FindDevicePushInfoCallback, (void*)&blFound,E_LOADER_USER);

	if (NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"SetDevicePushInfo execute error:%d ", nRet);
		return nRet;
	}

	if (blFound)
	{
		nRet = __UpdateDevicePushInfo(pInfo);
	}
	else
	{
		nRet = __InsertDevicePushInfo(pInfo);
	}

	return nRet;
}

//int GetInfoListCallback(void *param, int n_columm, char **column_value, char **column_name)
int db_get_dev_lst_cb(void *param, int n_columm, char **column_value, char **column_name)
{
	int nRet = NO_ERROR;
	SDevicePushInfoListReq *pListReq = (SDevicePushInfoListReq*)param;

	if (pListReq == NULL)
	{
		return nRet;
	}

	if (pListReq->pInfo == NULL)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"GetInfoListCallback pInfo is NULL, is only get info count: %d\n", pListReq->realLen);
	}
	else if (pListReq->length > pListReq->realLen)
	{
		int i = 0;
		for (i = 0; i < n_columm; i ++)
		{
			if (strcmp(column_name[i], "DeviceToken") == 0)
			{
				int nLen = sizeof(pListReq->pInfo[pListReq->realLen].deviceToken)/sizeof(char);
				if (strlen(column_value[i]) <= nLen)
				{
					strcpy(pListReq->pInfo[pListReq->realLen].deviceToken, column_value[i]);
				}
			}
			else if (strcmp(column_name[i], "MacAddress") == 0)
			{
				int nLen = sizeof(pListReq->pInfo[pListReq->realLen].macAddress)/sizeof(char);
				if (strlen(column_value[i]) <= nLen)
				{
					strcpy(pListReq->pInfo[pListReq->realLen].macAddress, column_value[i]);
				}
			}
			else if (strcmp(column_name[i], "OSType") == 0)
			{
				int nLen = sizeof(pListReq->pInfo[pListReq->realLen].osType)/sizeof(char);
				if (strlen(column_value[i]) <= nLen)
				{
					strcpy(pListReq->pInfo[pListReq->realLen].osType, column_value[i]);
				}
			}
			else if (strcmp(column_name[i], "Message") == 0)
			{
				int nLen = sizeof(pListReq->pInfo[pListReq->realLen].message)/sizeof(char);
				if (strlen(column_value[i]) <= nLen)
				{
          char escape_message[256];
          DecodeEscapeString(column_value[i], escape_message, 256);
					strcpy(pListReq->pInfo[pListReq->realLen].message, escape_message);
				}
			}
			else if (strcmp(column_name[i], "MonthlyMessage") == 0)
			{
				int nLen = sizeof(pListReq->pInfo[pListReq->realLen].monthlymessage)/sizeof(char);
				if (strlen(column_value[i]) <= nLen)
				{
          char escape_message[256];
          DecodeEscapeString(column_value[i], escape_message, 256);
					strcpy(pListReq->pInfo[pListReq->realLen].monthlymessage, escape_message);
				}
			}
			else if (strcmp(column_name[i], "PushTime") == 0)
			{
				int nLen = sizeof(pListReq->pInfo[pListReq->realLen].pushTime)/sizeof(char);
				if (strlen(column_value[i]) <= nLen)
				{
					strcpy(pListReq->pInfo[pListReq->realLen].pushTime, column_value[i]);
				}
			}
			else if (strcmp(column_name[i], "MonthlyPushTime") == 0)
			{
				int nLen = sizeof(pListReq->pInfo[pListReq->realLen].monthlypushTime)/sizeof(char);
				if (strlen(column_value[i]) <= nLen)
				{
					strcpy(pListReq->pInfo[pListReq->realLen].monthlypushTime, column_value[i]);
				}
			}
		}
	}
	pListReq->realLen++;

	return nRet;
}

//int GetDevicePushInfoListInter(SDevicePushInfoList *pList)
int db_get_dev_lst(SDevicePushInfoList *pList)
{
	int nRet = NO_ERROR;
	if (pList == NULL)
	{
		return nRet;
	}
	SDevicePushInfoListReq listReq;
	listReq.pInfo = pList->pInfo;
	listReq.length = pList->length;
	listReq.realLen = 0;

	char sqlCmd[SQLITE_COMMAND_LENGTH_MAX];
	sprintf(sqlCmd, "SELECT * FROM DevicePushInfo");

	nRet = db_exec_cmd(sqlCmd, db_get_dev_lst_cb, (void*)&listReq,E_LOADER_USER);
	if (nRet != NO_ERROR)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"GetDevicePushInfoList executable command error:%d \n", nRet);
		return nRet;
	}

	if (listReq.realLen > listReq.length)
	{
		nRet = NO_ERROR;//MY_SQLITE_BUFFER_SMALL;
	}
	pList->length = listReq.realLen;

	return nRet;
}

//int GetPushInfoCallback(void *param, int n_columm, char **column_value, char **column_name)
int db_get_dev_cb(void *param, int n_columm, char **column_value, char **column_name)
{
	int nRet = NO_ERROR;
	SDevicePushInfo *pInfo = (SDevicePushInfo*)param;

	if (pInfo == NULL)
	{
		return nRet;
	}

	int i = 0;
	for (i = 0; i < n_columm; i ++)
	{
		if (strcmp(column_name[i], "DeviceToken") == 0)
		{
			int nLen = sizeof(pInfo->deviceToken)/sizeof(char);
			if (strlen(column_value[i]) <= nLen)
			{
				strcpy(pInfo->deviceToken, column_value[i]);
			}
		}
		else if (strcmp(column_name[i], "MacAddress") == 0)
		{
			int nLen = sizeof(pInfo->macAddress)/sizeof(char);
			if (strlen(column_value[i]) <= nLen)
			{
				strcpy(pInfo->macAddress, column_value[i]);
			}
		}
		else if (strcmp(column_name[i], "OSType") == 0)
		{
			int nLen = sizeof(pInfo->osType)/sizeof(char);
			if (strlen(column_value[i]) <= nLen)
			{
				strcpy(pInfo->osType, column_value[i]);
			}
		}
		else if (strcmp(column_name[i], "Message") == 0)
		{
			int nLen = sizeof(pInfo->message)/sizeof(char);
			if (strlen(column_value[i]) <= nLen)
			{
        char escape_message[256];
        DecodeEscapeString(column_value[i], escape_message, 256);
				strcpy(pInfo->message, escape_message);
			}
		}
		else if (strcmp(column_name[i], "MonthlyMessage") == 0)
		{
			int nLen = sizeof(pInfo->monthlymessage)/sizeof(char);
			if (strlen(column_value[i]) <= nLen)
			{
        char escape_message[256];
        DecodeEscapeString(column_value[i], escape_message, 256);
				strcpy(pInfo->monthlymessage, escape_message);
			}
		}
		else if (strcmp(column_name[i], "PushTime") == 0)
		{
			int nLen = sizeof(pInfo->pushTime)/sizeof(char);
			if (strlen(column_value[i]) <= nLen)
			{
				strcpy(pInfo->pushTime, column_value[i]);
			}
		}
		else if (strcmp(column_name[i], "MonthlyPushTime") == 0)
		{
			int nLen = sizeof(pInfo->monthlypushTime)/sizeof(char);
			if (strlen(column_value[i]) <= nLen)
			{
				strcpy(pInfo->monthlypushTime, column_value[i]);
			}
		}
	}

	return nRet;
}

//int GetDevicePushInfoInter(const char deviceToken[128], SDevicePushInfo *pInfo)
int db_get_dev(const char deviceToken[128], SDevicePushInfo *pInfo)
{
	int nRet = NO_ERROR;
	char sqlCmd[SQLITE_COMMAND_LENGTH_MAX];

	OEM_LOG_INFO(OEM_LOG_LOW,"GetDevicePushInfoInter SQL 1\n");
	if (pInfo == NULL)
	{
		return FAIL;
	}

	sprintf(sqlCmd, "SELECT * FROM DevicePushInfo WHERE DeviceToken='%s'", deviceToken);

	nRet = db_exec_cmd(sqlCmd, db_get_dev_cb, (void*)pInfo,E_LOADER_USER);
	if (nRet != NO_ERROR)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"GetDevicePushInfo executable command error:%d \n", nRet);
	}

	return nRet;
}
//int UpdatePushTime(const char deviceToken[128], e_DevicePushInfoMsg_type message_type)
int db_update_push_time(const char deviceToken[128], e_DevicePushInfoMsg_type message_type)
{
	int nRet = NO_ERROR;

	char sqlCmd[SQLITE_COMMAND_LENGTH_MAX];
	time_t curTime;
	time(&curTime);

	char time[20];
	strftime(time, 20, "%Y-%m-%d %H:%M:%S",gmtime(&curTime));

  if (message_type == E_LIMIT_MSG)
	  sprintf(sqlCmd, "UPDATE DevicePushInfo SET PushTime='%s' WHERE DeviceToken='%s'", time, deviceToken);
  else if (message_type == E_MONTHLY_MSG)
	  sprintf(sqlCmd, "UPDATE DevicePushInfo SET MonthlyPushTime='%s' WHERE DeviceToken='%s'", time, deviceToken);

	nRet = db_exec_cmd(sqlCmd, NULL, NULL,E_LOADER_USER);
	if(NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"UpdatePushTime executable command error:%d \n", nRet);
	}

	return nRet;
}

//int DeleteDevicePushInfoInter(char mac_address[20])
int db_del_dev_by_mac(char mac_address[20])
{
	int nRet = NO_ERROR;
	char sqlCmd[SQLITE_COMMAND_LENGTH_MAX];

	sprintf(sqlCmd, "DELETE FROM DevicePushInfo WHERE MacAdress='%s'", mac_address);

	nRet = db_exec_cmd(sqlCmd, NULL, NULL,E_LOADER_USER);
	if(NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"db_exec_cmd execute failed, ret:%d", nRet);
	}

	return nRet;
}

//int DeleteInfoByDeviceTokenInter(char deviceToken[128])
int db_del_dev_by_token(const char deviceToken[128])
{
	int nRet = NO_ERROR;
	char sqlCmd[SQLITE_COMMAND_LENGTH_MAX];

	sprintf(sqlCmd, "DELETE FROM DevicePushInfo WHERE DeviceToken='%s'", deviceToken);

	nRet = db_exec_cmd(sqlCmd, NULL, NULL,E_LOADER_USER);
	if(NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"db_exec_cmd execute failed, ret:%d", nRet);
	}

	return nRet;
}
#if 0
{
	int nRet = NO_ERROR;
  SDevicePushInfo pInfo;

	pthread_mutex_lock(&database_mutex);
	nRet = OpenDevicePushInfoDB();
	if (MY_SQLITE_OK != nRet)
	{
		pthread_mutex_unlock(&database_mutex);
		OEM_LOG_INFO(OEM_LOG_ERROR,"OpenDevicePushInfoDB return error:%d", nRet);
		return nRet;
	}
	char sqlCmd[SQLITE_COMMAND_LENGTH_MAX];
	//sprintf(sqlCmd, "DELETE FROM DevicePushInfo WHERE DeviceToken='%s'", deviceToken);
	sprintf(sqlCmd, "SELECT * FROM DevicePushInfo WHERE DeviceToken='%s'", deviceToken);
	char *pErrMsg = NULL;
	//nRet = ExecuteCommand(pDeviceInfoDB, sqlCmd, NULL, NULL, pErrMsg);
	nRet = ExecuteCommand(pDeviceInfoDB, sqlCmd, GetPushInfoCallback, (void*)&pInfo, pErrMsg);
	if (nRet != MY_SQLITE_OK)
	{
		CloseDevicePushInfoDB();
		pthread_mutex_unlock(&database_mutex);
		OEM_LOG_INFO(OEM_LOG_ERROR,"GetDevicePushInfo executable command error:%d errorMsg:%s\n", nRet, pErrMsg);
		return nRet;
	}
	else
	{
    memset(pInfo.message, 0, sizeof(pInfo.message));
		nRet = UpdateDevicePushInfo(&pInfo);
	}
	CloseDevicePushInfoDB();
	pthread_mutex_unlock(&database_mutex);
	return nRet;
}
#endif


//int ClearPushTimeInter(e_DevicePushInfoMsg_type message_type)
int db_clear_push_time(e_DevicePushInfoMsg_type message_type)
{
	int nRet = NO_ERROR;
	char sqlCmd[SQLITE_COMMAND_LENGTH_MAX];

	//set update commond
	if (message_type == E_LIMIT_MSG)
		sprintf(sqlCmd, "UPDATE DevicePushInfo SET PushTime=''");
	else if (message_type == E_MONTHLY_MSG)
		sprintf(sqlCmd, "UPDATE DevicePushInfo SET MonthlyPushTime=''");

	nRet = db_exec_cmd(sqlCmd, NULL, NULL,E_LOADER_USER);
	if(NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR,"ClearPushTimeInter executable command error:%d \n", nRet);
	}

	return nRet;
}

//////
int db_insert_usage_history_data_internal(const usage_history_type *usage)
//int insert_usage_history_data(const usage_history_type *pHistoryData, char* imsi)
{
	char szSqlCmd[SQLITE_COMMAND_LENGTH_MAX];
	memset(szSqlCmd, 0, sizeof(szSqlCmd));

	if (NULL != usage)
	{
		sprintf(szSqlCmd, "INSERT INTO UsageHistory(StartTime, EndTime, RoamType, TxBytes, RxBytes) VALUES (%d, %d, %d, %llu, %llu)",
				usage->start_time, usage->end_time, usage->roam_type, usage->tx_bytes, usage->rx_bytes);
		OEM_LOG_INFO(OEM_LOG_LOW, "sql_cmd: %s\n", szSqlCmd);
	}
	else
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "param is setting errro!!\n");
		return FAIL;
	}

	return  db_exec_cmd(szSqlCmd, NULL, NULL,E_LOADER_USER);
}

int db_update_usage_history_data_internal(time_t old_end_time, const usage_history_type *usage)
//int update_usage_history_data(const usage_history_type *pHistoryData, char* imsi);
{
	char szSqlCmd[SQLITE_COMMAND_LENGTH_MAX] = {0};
	memset(szSqlCmd, 0, sizeof(szSqlCmd));

	if (NULL!=usage)
	{
		sprintf(szSqlCmd,
				"UPDATE UsageHistory SET TxBytes= %llu, RxBytes= %llu, RoamType=%d, EndTime=%d WHERE StartTime=%d and EndTime=%d",
				usage->tx_bytes, usage->rx_bytes, usage->roam_type, usage->end_time, usage->start_time, old_end_time);
		OEM_LOG_INFO(OEM_LOG_LOW, "sql_cmd: %s\n", szSqlCmd);
	}
	else
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "param is setting errro!!\n");
		return FAIL;
	}

	return db_exec_cmd(szSqlCmd, NULL, NULL,E_LOADER_USER);
}

static int db_get_usage_history_single_cb(void *param, int n_column, char **column_value, char **column_name)
{
	int i, nErr = NO_ERROR;
	usage_history_type *usage = (usage_history_type *)param;

	if (!usage) {
		return nErr;
	}
	for (i = 0; i < n_column; i++) {
		if (strcmp(column_name[i], "StartTime") == 0) {
//			strncpy(usage->start_date, column_value[i], DATETIME_STR_LEN);
			sscanf(column_value[i], "%d", &(usage->start_time));
		}
		else if (strcmp(column_name[i], "EndTime") == 0) {
//			strncpy(usage->end_date, column_value[i], DATETIME_STR_LEN);
			sscanf(column_value[i], "%d", &(usage->end_time));
		}
		else if (strcmp(column_name[i], "RoamType") == 0)
		{
			//usage->download = atof(column_value[i]);
			sscanf(column_value[i], "%d", &(usage->roam_type));
		}
		else if (strcmp(column_name[i], "TxBytes") == 0)
		{
			//usage->upload = atof(column_value[i]);
			sscanf(column_value[i], "%llu", &(usage->tx_bytes));
		}
		else if (strcmp(column_name[i], "RxBytes") == 0)
		{
			//usage->download = atof(column_value[i]);
			sscanf(column_value[i], "%llu", &(usage->rx_bytes));
		}
		else
		{
			//PERROR("Unknown column: %s.\n", column_name[0]);
		}
	}

	return nErr;
}
static int db_get_recoder_count_cb(void *param, int n_column, char ** column_value, char ** column_name)
{
	int ret = NO_ERROR;
	int i =0;
	int *pColumn_count = (int*) param;
	if(NULL == param)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "db_get_recoder_count_cb param setting is null!\n");
		return FAIL;
	}

	for(i = 0; i < n_column; ++i)
	{
		if(0 == strcmp(column_name[i], "column_count")){
			sscanf(column_value[i], "%d", pColumn_count);
		}
	}
	return NO_ERROR;
}
//
//get lst detail
static int db_get_usage_history_list_cb(void *param, int n_column, char **column_value, char **column_name)
{
	int nErr = NO_ERROR;
	int i;
	TUsageHistoryRecordListInter *pHistoryList = (TUsageHistoryRecordListInter *)param;

	if (!pHistoryList || !pHistoryList->pRecordDetail) {
		OEM_LOG_INFO(OEM_LOG_ERROR,"get_usage_history_list_callback, NULL parameter\n");
		return nErr;
	}

	if (pHistoryList->nRecordLen < pHistoryList->nRecordLenRet) {
		OEM_LOG_INFO(OEM_LOG_ERROR,"get_usage_history_list_callback, too many records\n");
		nErr = NO_ERROR;//MY_SQLITE_BUFFER_SMALL;
		return nErr;
	}

	for (i = 0; i < n_column; i ++) {
		if (strcmp(column_name[i], "TxBytes") == 0) {
			//pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].download = atof(column_value[i]);
			sscanf(column_value[i], "%llu", &(pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].tx_bytes));
		} else if (strcmp(column_name[i], "RxBytes") == 0) {
			//pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].upload = atof(column_value[i]);
			sscanf(column_value[i], "%llu", &(pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].rx_bytes));
		} else if (strcmp(column_name[i], "RoamType") == 0) {
			//pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].download = atof(column_value[i]);
			sscanf(column_value[i], "%d", &(pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].roam_type));
		} else if (strcmp(column_name[i], "StartTime") == 0) {
			//pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].upload = atof(column_value[i]);
			//strncpy(pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].start_date,
			//	column_value[i], DATETIME_STR_LEN);
			sscanf(column_value[i], "%d", &(pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].start_time));
		} else if (strcmp(column_name[i], "EndTime") == 0) {
			//			strncpy(pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].end_date,
			//				column_value[i], DATETIME_STR_LEN);
			sscanf(column_value[i], "%d", &(pHistoryList->pRecordDetail[pHistoryList->nRecordLenRet].end_time));
		} else {
			//PDEBUG("Unknown column: %s\n", column_name[i]);
		}
	}
	pHistoryList->nRecordLenRet++;


	return nErr;
}

//int get_usage_history_records(const usage_history_date_dur_type *interval,TUsageHistoryRecordListInter *usageListRecord)
//get history total usage
int db_get_usage_total_num_data(const usage_history_time_dur_type *interval, TUsageHistoryRecordListInter *usageListRecord)
//int get_usage_data_detail(const usage_history_date_dur_type *interval, usage_history_type *usage);
{
	int nErr = NO_ERROR;
	char szSqlCmd[SQLITE_COMMAND_LENGTH_MAX]={0};
	int column_count = 0;

	//query the count of recouder between StartTime and Endtime
	if(NULL == interval)
	{
		sprintf(szSqlCmd, "select count(StartTime) as column_count from UsageHistory");
	}
	else
	{
		/*begin add by wei.huang.sz add usage 2016-1-25*/
		sprintf(szSqlCmd, "select count(StartTime) as column_count from UsageHistory where EndTime<=%d and StartTime>=%d",interval->end_time ,  interval->start_time);
		/*end add by wei.huang.sz add usage 2016-1-25*/
	}
	nErr = db_exec_cmd(szSqlCmd, db_get_recoder_count_cb, &column_count,E_LOADER_USER);
	if(NO_ERROR != nErr)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "get table's row failed !\n");
		return FAIL;
	}
	if(0 >= column_count)
	{
		/*begin add by wei.huang.sz add usage 2016-1-25*/
		//OEM_LOG_INFO(OEM_LOG_ERROR, "get table's rows is 0 !\n");
		/*end add by wei.huang.sz add usage 2016-1-25*/
	}
  else
  {
  	//malloc memery from heap
  	usageListRecord->pRecordDetail = calloc(column_count, sizeof(usage_history_type));
	usageListRecord->nRecordLen = column_count;
  	if(NULL == usageListRecord->pRecordDetail)
  	{
  		OEM_LOG_INFO(OEM_LOG_ERROR, "calloc memery failed !\n");
  		return FAIL;
  	}


  	if(NULL == interval)
  	{
  		sprintf(szSqlCmd, "select * from UsageHistory ");
  	}
  	else
  	{
		/*begin add by wei.huang.sz add usage 2016-1-25*/
  		sprintf(szSqlCmd, "select * from UsageHistory WHERE EndTime<=%d and StartTime>=%d ", interval->end_time,  interval->start_time);
		/*end add by wei.huang.sz add usage 2016-1-25*/
  	}

  	//	nErr = ExecuteCommand(pStatisticsDB, szSqlCmd, get_usage_history_list_callback, usageListRecord, NULL);
  	nErr = db_exec_cmd(szSqlCmd, db_get_usage_history_list_cb, usageListRecord,E_LOADER_USER);

  	OEM_LOG_INFO(OEM_LOG_LOW,"(SQL:%d) %s\n", nErr, szSqlCmd);

    if(NULL != interval)
    {
    	//OEM_LOG_INFO(OEM_LOG_LOW,"get_usage_history_records from %d to %d, result: %d\n",
    			//interval->start_time, interval->end_time, usageListRecord->nRecordLenRet);
    }
  }
	return nErr;
}
/*
 * delete the recoder befor EndTime
 * delete all of the recoder when usage is null
 */
int db_del_usage_history_record(time_t date)
{
	char szSqlCmd[SQLITE_COMMAND_LENGTH_MAX];

	memset(szSqlCmd, 0, sizeof(szSqlCmd));

	if (!date) {
		sprintf(szSqlCmd, "DELETE FROM UsageHistory");
	}
	else
	{
	//delete the recorder before EndTime
		sprintf(szSqlCmd, "DELETE FROM UsageHistory WHERE EndTime<=%d",	date);
		OEM_LOG_INFO(OEM_LOG_LOW,"(SQL: %s\n", szSqlCmd);
	}

	return db_exec_cmd(szSqlCmd, NULL, NULL,E_LOADER_USER);
}
