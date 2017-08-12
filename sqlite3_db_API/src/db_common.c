
/*
****************************************************************************
* FILE        :
*                                 qlite_db_operationi.c
* DESCRIPTION :
*                    SQLITE3 DATABASE MODULE
****************************************************************************
*    Copyright (c) 2015 by COM Incorporated.  All Rights Reserved.
****************************************************************************
*/
/*
**********************************EditHistory*******************************
* This section contains comments describing changes made to the module.
* Notice that changes are listed in reverse chronological order.
*
*    when               who                      what, where, why
* ------------    ---------------    ---------------------------------------
*  20XX/XX/XX        Name.name           Create the initial version.
*
****************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <pthread.h>
#include "db_common.h"
#include "db.h"
#include "db_usage_sqlite3.h"

#define DATABASE_BAKEUP_FILE "/j-resource/resource/sqlite3/network_bk.db3"
#define DATABASE_USER_FILE "/j-resource/resource/sqlite3/user_info.db3"
#define DATABASE_FACTORY_FILE "/j-resource/resource/sqlite3/factory_info.db3"
#define DATABASE_RESTORE_FILE "/tmp/backup_dir/user_info.db3"

struct tbl_name{
	char tbl_name[32];
};

struct tbl_info{
	struct tbl_name *tbl_lst;
	int cur_count;
	int max_count;
};

struct item{
	char item_name[128];
	char item_value[512];
	e_paramtype_type type;
};

struct item_info{
	struct item *item_lst;
	int cur_count;
	int max_count;
};

typedef struct {
    char    start_time[128];
    char    end_time[128];
    char    roam_type[16];
    char    tx_bytes[128];
    char    rx_bytes[128];
} usage;

struct usage_info{
	usage_history_type *usage_lst;
	int cur_count;
	int max_count;
};


static pthread_mutex_t db_mutex;

static sqlite3 *usr_db=NULL;
static sqlite3 *fac_db=NULL;
static sqlite3 *rest_db=NULL;

static int safe_atoi(const char * nptr)
{
	if(NULL == nptr)
		return 0;
	return atoi(nptr);
}
static long long  safe_atoll(const char * nptr)
{
	if(NULL == nptr)
		return 0;
	return atoll(nptr);
}

static sqlite3 * open_sqlite_db(e_loder_sqlite load_type)
{
	char **azResult;
	sqlite3 **db=NULL;
	char *zErrMsg = 0;
	char sys_cmd[256]={0};
	int rc = SQLITE_OK;

	if(access(DATABASE_USER_FILE, 0) == -1)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, db:%s couldn't access.\n", DATABASE_USER_FILE);
		if(access(DATABASE_FACTORY_FILE, 0) == -1)
		{
			OEM_LOG_INFO(OEM_LOG_ERROR, "Error, db:%s couldn't access.\n", DATABASE_FACTORY_FILE);
			return NULL;
		}
		sprintf(sys_cmd, "cp %s %s", DATABASE_FACTORY_FILE, DATABASE_USER_FILE);
		system(sys_cmd);
	}

	if(E_LOADER_FACTORY == load_type)
	{
	  if(usr_db)
	  {
	    sqlite3_close(usr_db);
		usr_db=NULL;
	  }
	  if(rest_db)
	  {
	    sqlite3_close(rest_db);
		rest_db=NULL;
	  }
	  if(!fac_db)
		  rc = sqlite3_open(DATABASE_FACTORY_FILE, &fac_db);
		db = &fac_db;
	}
	else if(E_LOADER_USER == load_type)
	{
	  if(fac_db)
	  {
	    sqlite3_close(fac_db);
		fac_db=NULL;

	  }
	  if(rest_db)
	  {
	    sqlite3_close(rest_db);
		rest_db=NULL;
	  }
	  if(!usr_db)
		  rc = sqlite3_open(DATABASE_USER_FILE, &usr_db);
		db = &usr_db;
	}
	else
	{
		if(usr_db)
		{
		  sqlite3_close(usr_db);
		  usr_db=NULL;
		}
		if(fac_db)
		{
		  sqlite3_close(fac_db);
		  fac_db=NULL;

		}
		if(!rest_db)
			rc = sqlite3_open(DATABASE_RESTORE_FILE, &rest_db);
		 db = &rest_db;
	}


	if( rc != SQLITE_OK)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, db:%s open failed:%s.\n", DATABASE_USER_FILE, sqlite3_errmsg(*db));
		sqlite3_close(*db);
		*db = NULL;
		return NULL;
	}
	return *db;
}

static int close_sqlite_db(void)
{
	int rc = SQLITE_OK;

	if(usr_db)
	{
		rc = sqlite3_close(usr_db);
		usr_db=NULL;
	}

	return 0;
}

static char ** search_by_table(const char* sql_cmd, int *nrow, int *ncolumn, e_loder_sqlite load_type)
{
	char **azResult;
	sqlite3 *db=NULL;
	char *zErrMsg = 0;
	int rc;

	//open database
	db = open_sqlite_db(load_type);
	if(NULL == db)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, open_sqlite_db return faild.\n");
		return NULL;
	}

	//query database
	rc = sqlite3_get_table( db , sql_cmd, &azResult , nrow , ncolumn , &zErrMsg );
	if( rc != SQLITE_OK)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, sqlite3_get_table run faild, sql_cmd:[%s], db err:[%s].\n", sql_cmd, sqlite3_errmsg(db));
		if(zErrMsg)
		{
			OEM_LOG_INFO(OEM_LOG_ERROR, "Error, sql_cmd err info:[%s].\n",  zErrMsg);
		}
		sqlite3_free_table( azResult );
		//sqlite3_close(db);
		return NULL;
	}

	//close database
	//sqlite3_close(db);
	return azResult;
}
static int __db_query_db_init_struct(param_table_info_t *tbl_info)
{
	int rc = NO_ERROR;
	int i = 0;
	int nrow = 0;
	int ncolumn = 0;
	char ** azResult;
	char sql_cmd[256];
	char* tbl_name  = NULL;
	param_key_info_t *param_table = NULL;

	//table setting error
	if((NULL == tbl_info)||(NULL == tbl_info->table_name)||(NULL == tbl_info->param_table))
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, tbl_name is null. \n");
		return FAIL;
	}
	tbl_name = tbl_info->table_name;
	param_table = tbl_info->param_table;

	for(i = 0; param_table[i].key != NULL; ++i)
	{
		sprintf(sql_cmd, "select value from %s where items = '%s'", tbl_name, param_table[i].key);
		OEM_LOG_INFO(OEM_LOG_LOW, "rc:%d, search_by_table sql_cmd:%s\n", rc, sql_cmd);
		azResult = search_by_table(sql_cmd, &nrow, &ncolumn, false);
		OEM_LOG_INFO(OEM_LOG_LOW, "rc:%d,  search_by_table azResult:%p, nrow:%d\n", rc, azResult, nrow);
		if((NULL == azResult)||(0 == nrow))
		{
			OEM_LOG_INFO(OEM_LOG_ERROR, "Error, db:%s search_by_table return failed. sql_cmd:%s\n",  DATABASE_USER_FILE, sql_cmd);
			//return FAIL;
			continue;
		}


		switch(param_table[i].param_type)
		{
			case E_PARAM_U8:
				*((uint8*)(param_table[i].param)) = safe_atoi(azResult[1]);
				break;
			case E_PARAM_BOOL:
				*((boolean*)(param_table[i].param)) = (boolean)safe_atoi(azResult[1]);
				break;
			case E_PARAM_U16:
				*((uint16*)(param_table[i].param)) = (uint16)safe_atoi(azResult[1]);
				break;
			case E_PARAM_INT:
			case E_PARAM_ENUM:
				*((int*)(param_table[i].param)) = safe_atoi(azResult[1]);
				break;
			case E_PARAM_U64:
				*((uint64*)(param_table[i].param)) = safe_atoll(azResult[1]);
				break;
			case E_PARAM_STR:
				memset(param_table[i].param, 0, param_table[i].param_len);
				STRNCPY((char*)(param_table[i].param), azResult[1], param_table[i].param_len-1);
				break;
			default:
				OEM_LOG_INFO(OEM_LOG_ERROR, "Error, param_type:%d \n",  param_table[i].param_type);
				break;
		}
		//free tmp memery
		sqlite3_free_table( azResult );
		azResult = NULL;
	}
	return rc;
}

static int db_get_count_cb(void *param, int n_column, char ** column_value, char ** column_name)
{
	int ret = 0;//NO_ERROR;
	int i =0;
	int *pColumn_count = (int*) param;
	if(NULL == param)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "db_get_recoder_count_cb param setting is null!\n");
		return FAIL;
	}
	printf("fun:%s\n", __FUNCTION__);

	for(i = 0; i < n_column; ++i)
	{
		sscanf(column_value[i], "%d", pColumn_count);
	}

	return NO_ERROR;
}

static int update_item_to_db(char * sql_cmd)
{
	int rc = 0;
	char *pErrMsg= 0;
	sqlite3 *db = NULL;

	//open database
	db = open_sqlite_db(E_LOADER_USER);
	if(NULL == db)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, db:%s open_sqlite_db return faild.\n", DATABASE_USER_FILE);
		return FAIL;
	}

	rc = sqlite3_exec(db, sql_cmd, 0, 0, &pErrMsg);

	//sqlite3_close(db);

	if(rc != SQLITE_OK){
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, sqlite3_get_table run faild, sql_cmd:[%s], db err:[%s].\n",
				sql_cmd, sqlite3_errmsg(db));
		if(pErrMsg)
		{
			OEM_LOG_INFO(OEM_LOG_ERROR, "Error, sql_cmd err info:[%s].\n",  pErrMsg);
		}
		return FAIL;
	}
	return NO_ERROR;
}

int db_exec_cmd(char * sql_cmd, sqlite3_callback callbackFun, void *pUserData, e_loder_sqlite type)
{
	char *pErrMsg= 0;
	int rc = 0;
	sqlite3 * db=NULL;
	//open database
	db = open_sqlite_db(type);
	if(NULL == db)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, db:%s open_sqlite_db return faild.\n", DATABASE_USER_FILE);
		return FAIL;
	}

	OEM_LOG_INFO(OEM_LOG_LOW, "executeCommand sql_cmd:%s!!\n", sql_cmd);
	rc = sqlite3_exec(db, sql_cmd, callbackFun, pUserData, &pErrMsg);

	//sqlite3_close(db);

	if(rc != SQLITE_OK){
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, sqlite3_get_table run faild, sql_cmd:[%s], db err:[%s].\n",
				sql_cmd, sqlite3_errmsg(db));
		if(pErrMsg)
		{
			OEM_LOG_INFO(OEM_LOG_ERROR, "Error, sql_cmd err info:[%s].\n",  pErrMsg);
		}
		return FAIL;
	}
	return NO_ERROR;
}

//update database
static int update_item_string(char * tbl, char *item, char *value)
{
	char sql_cmd[256];
	sprintf(sql_cmd, "update %s set value='%s' where items = '%s';",
			tbl, value, item);
	return update_item_to_db(sql_cmd);
}
//update database  with int
static int update_item_int(char * tbl, char *item, int value)
{
	char sql_cmd[256];

	sprintf(sql_cmd, "update %s set value='%d' where items = '%s';",
			tbl, value, item);
	return update_item_to_db(sql_cmd);
}
static int update_item_uint64(char * tbl, char *item, uint64 value)
{
	char sql_cmd[256];

	sprintf(sql_cmd, "update %s set value='%llu' where items = '%s';",
			tbl, value, item);
	return update_item_to_db(sql_cmd);
}
/*
 *
 *
 *
 */
static int __db_update_db_from_struct(param_table_info_t *tbl_info, int id)
{
	int i = 0;
	int rc = NO_ERROR;
	char* tbl_name  = NULL;
	param_key_info_t *param_table = NULL;

	//table setting error
	if((NULL == tbl_info)||(NULL == tbl_info->table_name)||(NULL == tbl_info->param_table))
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, tbl_name is null. \n");
		return FAIL;
	}
	tbl_name = tbl_info->table_name;
	param_table = tbl_info->param_table;

	for(i = 0; param_table[i].key != NULL; ++i)
	{
		if((id == -1) ||(id == param_table[i].param_id))
		{
			switch(param_table[i].param_type)
			{
				case E_PARAM_U8:
					rc = update_item_int(tbl_name, param_table[i].key, *((uint8*)(param_table[i].param)));
					break;
				case E_PARAM_BOOL:
					rc = update_item_int(tbl_name, param_table[i].key, *((boolean*)(param_table[i].param)));
					break;
				case E_PARAM_INT:
				case E_PARAM_ENUM:
					rc = update_item_int(tbl_name, param_table[i].key, *((int*)(param_table[i].param)));
					break;
				case E_PARAM_U64:
					rc = update_item_uint64(tbl_name, param_table[i].key, *((uint64*)(param_table[i].param)));
					break;
				case E_PARAM_STR:
					rc = update_item_string(tbl_name, param_table[i]. key, param_table[i].param);
					break;
				default:
					OEM_LOG_INFO(OEM_LOG_ERROR, "Error, param_type:%d \n",  param_table[i].param_type);
					break;
			}
			if(rc != 0)
			{
				OEM_LOG_INFO(OEM_LOG_ERROR, "Error, tbl_name:%s update key:[%s] failed.\n", tbl_name, param_table[i].key);
				break;
			}

		}
	}
	/*added by yuegui.he*/
	sync();
	return rc;
}
static char * __db_get_factory_value(char *tbl_name, char *key)
{
	char **azResult; //save record
	char sql_cmd[256];
	int nrow = 0;
	int ncolumn = 0;
	if(NULL == tbl_name)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, tbl_name is null. \n");
		return NULL;
	}
	sprintf(sql_cmd, "select value from %s where items = '%s'", tbl_name, key);
	azResult = search_by_table(sql_cmd, &nrow, &ncolumn, true);
	if((NULL == azResult)||(0 == nrow))
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, db:%s search_by_table return failed.\n",  DATABASE_USER_FILE);
		return NULL;
	}
	return azResult;
}
static int __db_reload_value(param_table_info_t *tbl_info, int id)
{
	char sql_cmd[256];
	char **pResult;
	int i = 0;
	char* tbl_name  = NULL;
	param_key_info_t *param_table = NULL;

	//table setting error
	if((NULL == tbl_info)||(NULL == tbl_info->table_name)||(NULL == tbl_info->param_table))
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, tbl_name is null. \n");
		return FAIL;
	}
	memset(sql_cmd, 0, sizeof(sql_cmd));
	tbl_name = tbl_info->table_name;
	param_table = tbl_info->param_table;

	for(i = 0; param_table[i].key != NULL; ++i)
	{
		if(id == i)
		{
			pResult = __db_get_factory_value(tbl_name, param_table[i].key);
			if(NULL == pResult)
				continue;
			switch(param_table[i].param_type)
			{
				case E_PARAM_U8:
					*((uint8*)(param_table[i].param)) = safe_atoi(pResult[1]);
					break;
				case E_PARAM_BOOL:
					*((boolean*)(param_table[i].param)) = (boolean)safe_atoi(pResult[1]);
					break;
				case E_PARAM_U16:
					*((uint16*)(param_table[i].param)) = (uint16)safe_atoi(pResult[1]);
					break;
				case E_PARAM_INT:
				case E_PARAM_ENUM:
					*((int*)(param_table[i].param)) = safe_atoi(pResult[1]);
					break;
				case E_PARAM_U64:
					*((uint64*)(param_table[i].param)) = safe_atoll(pResult[1]);
					break;
				case E_PARAM_STR:
					memset(param_table[i].param, 0, param_table[i].param_len);
					STRNCPY((char*)(param_table[i].param), pResult[1], param_table[i].param_len-1);
					break;
				default:
					OEM_LOG_INFO(OEM_LOG_ERROR, "Error, param_type:%d \n",  param_table[i].param_type);
					break;
			}
			sprintf(sql_cmd, "update %s set value='%s' where items='%s'", tbl_name, pResult[1], param_table[i].key);
			sqlite3_free_table(pResult);
			pResult= NULL;
		}
	}
	if(0 == sql_cmd[0])
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, PARAM TYPE SETING ERROR.\n");
		return -1;
	}

	return update_item_to_db(sql_cmd);
}
static int __db_get_value(char *tbl_name, char *key, void*value, e_paramtype_type type, int len)
{
	char **azResult; //save record
	char sql_cmd[256];
	int nrow = 0;
	int ncolumn = 0;
	if(NULL == tbl_name)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, tbl_name is null. \n");
		return -1;
	}
	sprintf(sql_cmd, "select value from %s where items = '%s'", tbl_name, key);
	azResult = search_by_table(sql_cmd, &nrow, &ncolumn, false);
	if((NULL == azResult)||(0 == nrow))
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, db:%s search_by_table return failed.\n",  DATABASE_USER_FILE);
		return -1;
	}

	switch(type)
	{
		case E_PARAM_U8:
			*((uint8*)value) = (boolean)safe_atoi(azResult[1]);
			break;
		case E_PARAM_BOOL:
			*((boolean*)value) = (boolean)safe_atoi(azResult[1]);
			break;
		case E_PARAM_INT:
		case E_PARAM_U32:
		case E_PARAM_ENUM:
			*((int*)value) = safe_atoi(azResult[1]);
			break;
		case E_PARAM_U64:
			*((uint64*)value) = safe_atoll(azResult[1]);
			break;
		case E_PARAM_STR:
			memset(value, 0, len);
			STRNCPY((char*)value, azResult[1], len-1);
			break;
		default:
			OEM_LOG_INFO(OEM_LOG_ERROR, "Error, param_type:%d \n",  type);
			break;
	}
	//free tmp memery
	sqlite3_free_table( azResult );
	return NO_ERROR;
}

static int __db_insert_record(char *tbl_name,  char *key, void*value, e_paramtype_type type)
{
	char sql_cmd[256];
	if(NULL == tbl_name)
	{
		return -1;
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, tbl_name is null. \n");
	}

	memset(sql_cmd, 0, sizeof(sql_cmd));

	switch(type)
	{
		case E_PARAM_U8:
			sprintf(sql_cmd, "insert into %s(items,value ) values('%s', '%d');",
					tbl_name, key, *((uint8*)value));
			break;
		case E_PARAM_BOOL:
			sprintf(sql_cmd, "insert into %s(items,value) values('%s', '%d');",
					tbl_name, key, *((boolean*)value));
			break;
		case E_PARAM_INT:
		case E_PARAM_ENUM:
			sprintf(sql_cmd, "insert into %s(items,value) values('%s', '%d');",
					tbl_name, key, *((int*)value));
			break;
		case E_PARAM_U64:
			sprintf(sql_cmd, "insert into %s(items,value) values('%s', '%llu');",
					tbl_name, key, *((uint64*)value));
			break;
		case E_PARAM_STR:
			sprintf(sql_cmd, "insert into %s(items,value) values('%s', '%s');",
					tbl_name, key,  (char*)value, (char*)value);
			break;
		default:
			OEM_LOG_INFO(OEM_LOG_ERROR, "Error, param_type:%d \n",  type);
			break;
	}
	if(0 == sql_cmd[0])
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, PARAM TYPE SETING ERROR.\n");
		return -1;
	}
	return update_item_to_db(sql_cmd);
}


static int __db_update_value(char *tbl_name, char *key, void*value, e_paramtype_type type)
{
	char sql_cmd[256];
	if(NULL == tbl_name)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, tbl_name is null. \n");
		return -1;
	}
	memset(sql_cmd, 0, sizeof(sql_cmd));

	switch(type)
	{
		case  E_PARAM_U8:
			sprintf(sql_cmd, "update %s set value='%d' where items = '%s';", tbl_name, *((uint8*)value), key);
			break;
		case  E_PARAM_BOOL:
			sprintf(sql_cmd, "update %s set value='%d' where items = '%s';", tbl_name, *((boolean*)value), key);
			break;
		case E_PARAM_INT:
		case E_PARAM_ENUM:
			sprintf(sql_cmd, "update %s set value='%d' where items = '%s';", tbl_name, *((int*)value), key);
			break;
		case E_PARAM_U64:
			sprintf(sql_cmd, "update %s set value='%llu' where items = '%s';", tbl_name, *((uint64*)value), key);
			break;
		case E_PARAM_STR:
			sprintf(sql_cmd, "update %s set value='%s' where items = '%s';", tbl_name,(char*)value, key);
			break;
		default :
			OEM_LOG_INFO(OEM_LOG_ERROR, "Error, param_type:%d \n",  type);
			break;
	}

	if(0 == sql_cmd[0])
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, PARAM TYPE SETING ERROR.\n");
		return -1;
	}

	return update_item_to_db(sql_cmd);
}
static int __db_delete_record(char *tbl_name, char *key, void*value, e_paramtype_type type)
{
	char sql_cmd[256];
	if(NULL == tbl_name)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, tbl_name is null. \n");
		return -1;
	}

	memset(sql_cmd, 0, sizeof(sql_cmd));
	//delete from usim where items='byh';
	switch(type)
	{
		case E_PARAM_U8:
			sprintf(sql_cmd, "delete from %s where items='%d'", tbl_name, *((uint8*)value));
			break;
		case E_PARAM_BOOL:
			sprintf(sql_cmd, "delete from %s where items='%d'", tbl_name, *((boolean*)value));
			break;
		case E_PARAM_INT:
		case E_PARAM_ENUM:
			sprintf(sql_cmd, "delete from %s where items='%d'", tbl_name, *((int*)value));
			break;
		case E_PARAM_U64:
			sprintf(sql_cmd, "delete from %s where items='%llu'", tbl_name, *((uint64*)value));
			break;
		case E_PARAM_STR:
			sprintf(sql_cmd, "delete from %s where items='%s'", tbl_name, (char*)value);
			break;
		default:
			OEM_LOG_INFO(OEM_LOG_ERROR, "Error, param_type:%d \n",  type);
			break;
	}

	if(0 == sql_cmd[0])
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "Error, PARAM TYPE SETING ERROR.\n");
		return -1;
	}
	return update_item_to_db(sql_cmd);
}

/*
 * get table list of database
 *
 */
static int __db_get_item(char *tbl_name, struct item_info *P_item_info, e_loder_sqlite type)
{
	int nRet = 0;
	int indix;
	int i, j;
	char sql_cmd[256]={0};
	char **azResult; //save record
    int nrow = 0;
	int ncolumn = 0;

	sprintf(sql_cmd, "select *from %s;", tbl_name);
    azResult = search_by_table(sql_cmd, &nrow, &ncolumn, type);
    indix = ncolumn;

	for(i = 0; i < nrow; i++)
	{
		for(j = 0; j < ncolumn; j++)
		{
			if(0 == strcmp( azResult[j], "items"))
			{
				strncpy(P_item_info->item_lst[P_item_info->cur_count].item_name,azResult[indix],sizeof(P_item_info->item_lst[P_item_info->cur_count].item_name));
			}
			else
			{
				strncpy(P_item_info->item_lst[P_item_info->cur_count].item_value,azResult[indix],sizeof(P_item_info->item_lst[P_item_info->cur_count].item_value));
				P_item_info->item_lst[P_item_info->cur_count].type = E_PARAM_STR;
				P_item_info->cur_count++;
			}
			indix++;
		}
	}

	return nRet;

}

static int __db_get_usage_item(char *tbl_name, struct usage_info *p_usage_info, e_loder_sqlite type)
{
	int nRet = 0;
	int indix;
	int i, j;
	char sql_cmd[256]={0};
	char **azResult; //save record
    int nrow = 0;
	int ncolumn = 0;

	sprintf(sql_cmd, "select *from %s;", tbl_name);
    azResult = search_by_table(sql_cmd, &nrow, &ncolumn, type);
    indix = ncolumn;

	for(i = 0; i < nrow; i++)
	{
		for(j = 0; j < ncolumn; j++)
		{
			switch((indix % ncolumn))
			{
				case 0:
					sscanf(azResult[indix], "%d", &(p_usage_info->usage_lst[p_usage_info->cur_count].start_time));
					break;
				case 1:
					sscanf(azResult[indix], "%d", &(p_usage_info->usage_lst[p_usage_info->cur_count].end_time));
					break;
				case 2:
					sscanf(azResult[indix], "%d", &(p_usage_info->usage_lst[p_usage_info->cur_count].roam_type));
					break;
				case 3:
					sscanf(azResult[indix], "%llu", &(p_usage_info->usage_lst[p_usage_info->cur_count].tx_bytes));
					break;
				case 4:
					sscanf(azResult[indix], "%llu", &(p_usage_info->usage_lst[p_usage_info->cur_count].rx_bytes));
					p_usage_info->cur_count++;
					break;
			}
			indix++;
		}
	}

    return nRet;
}

static int __db_get_tabl_list(struct tbl_info *p_tbl_info, e_loder_sqlite type)
{
	int nRet = 0;
	char szSqlCmd[128]={0};
	int column_count = 0;

	sprintf(szSqlCmd, "select count(name) as column_count from sqlite_master where type='table';");
	nRet = db_exec_cmd(szSqlCmd, db_get_count_cb, &column_count, type);
	if(NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "get table's row failed !\n");
		return FAIL;
	}

	if(0 >= column_count)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "get table count failed\n");
		return FAIL;
	}
	else
	{
		//malloc memery from heap
		p_tbl_info->tbl_lst= calloc(column_count, sizeof(struct tbl_name));
		if(NULL == p_tbl_info->tbl_lst)
		{
			OEM_LOG_INFO(OEM_LOG_ERROR, "calloc memery failed !\n");
			return -1;
		}
		p_tbl_info->cur_count = 0;
		p_tbl_info->max_count = column_count;
	}

	nRet = db_get_table(p_tbl_info, type);

	return nRet;
}

static int __db_get_item_list(char *tbl_name, struct item_info *p_item_info, e_loder_sqlite type)
{
    int nRet = 0;
	int indix;
	int i, j;
	char sql_cmd[256]={0};
	char **azResult; //save record
    int nrow = 0;
	int ncolumn = 0;

	sprintf(sql_cmd, "select count(*) from %s;", tbl_name);
	nRet = db_exec_cmd(sql_cmd, db_get_count_cb, &ncolumn, type);
	if(NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "get table's row failed !\n");
		return FAIL;
	}

	if(0 >= ncolumn)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "get table count  %d!\n", ncolumn);
		return -1;
	}
	else
	{
		//malloc memery from heap
		p_item_info->item_lst= calloc(ncolumn, sizeof(struct item));
		if(NULL == p_item_info->item_lst)
		{
			OEM_LOG_INFO(OEM_LOG_ERROR, "calloc memery failed !\n");
			return -1;
		}
		p_item_info->cur_count = 0;
		p_item_info->max_count = ncolumn;
	}

	nRet = __db_get_item(tbl_name, p_item_info, type);

	return nRet;
}

static int __db_get_item_usagelist(char *tbl_name, struct usage_info *p_usage_info, e_loder_sqlite type)
{
    int nRet = 0;
	int indix;
	int i, j;
	char sql_cmd[256]={0};
	char **azResult; //save record
    int nrow = 0;
	int ncolumn = 0;

	sprintf(sql_cmd, "select count(*) from %s;", tbl_name);
	nRet = db_exec_cmd(sql_cmd, db_get_count_cb, &ncolumn, type);
	if(NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "get table's row failed !\n");
		return FAIL;
	}

	if(0 >= ncolumn)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "get table count %d\n", ncolumn);
		p_usage_info->cur_count = 0;
		p_usage_info->max_count = 0;
		p_usage_info->usage_lst = NULL;
		return 0;
	}
	else
	{
		//malloc memery from heap
		p_usage_info->usage_lst= calloc(ncolumn, sizeof(usage_history_type));
		if(NULL == p_usage_info->usage_lst)
		{
			OEM_LOG_INFO(OEM_LOG_ERROR, "calloc memery failed !\n");
			return -1;
		}
		p_usage_info->cur_count = 0;
		p_usage_info->max_count = ncolumn;
	}

	nRet = __db_get_usage_item(tbl_name, p_usage_info, type);

	return nRet;
}


/*
 * init varible of tbl_info from database
 *
 */
int db_query_db_init_struct(param_table_info_t *tbl_info)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = __db_query_db_init_struct(tbl_info) ;
	UNLOCK_EX(&db_mutex);
	return rc;
}


int db_close_sqlite(void)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = close_sqlite_db() ;
	UNLOCK_EX(&db_mutex);
	return rc;
}


/*
 * update key's value to database, update recode of tbl_info with id when id setting, otherwish update all of the key's value of tbl_info
 */
int db_update_db_from_struct(param_table_info_t *tbl_info, int id)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = __db_update_db_from_struct(tbl_info, id) ;
	UNLOCK_EX(&db_mutex);
	return rc;
}
/*
 * description: get the value of key from tbl_name
 */
int db_get_value(char *tbl_name, char *key, void*value, e_paramtype_type type, int len)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = __db_get_value(tbl_name, key, value, type, len) ;
	UNLOCK_EX(&db_mutex);
	return rc;
}
int db_insert_record(char *tbl_name,  char *key, void*value, e_paramtype_type type)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = __db_insert_record(tbl_name, key, value, type) ;
	UNLOCK_EX(&db_mutex);
	return rc;
}

//int db_reload_value(char *tbl_name, char *key, e_paramtype_type type)
int db_reload_value(param_table_info_t *tbl_info, int id)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = __db_reload_value(tbl_info, id);
	UNLOCK_EX(&db_mutex);
	return rc;
}
int db_update_value(char *tbl_name, char *key, void*value, e_paramtype_type type)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = __db_update_value(tbl_name, key, value, type) ;
	UNLOCK_EX(&db_mutex);
	return rc;
}
int db_delete_record(char *tbl_name, char *key, void*value, e_paramtype_type type)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = __db_delete_record(tbl_name, key, value, type) ;
	UNLOCK_EX(&db_mutex);
	return rc;
}

int db_get_tabl_list(struct tbl_info *p_tbl_info, e_loder_sqlite type)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = __db_get_tabl_list(p_tbl_info,type);
	UNLOCK_EX(&db_mutex);
	return rc;
}

int db_get_item_list(char *tbl_name, struct item_info *p_item_info, e_loder_sqlite type)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = __db_get_item_list(tbl_name,p_item_info,type);
	UNLOCK_EX(&db_mutex);
	return rc;
}

int db_get_item_usagelist(char *tbl_name, struct usage_info *p_usage_info, e_loder_sqlite type)
{
	int rc = NO_ERROR;
	LOCK_EX(&db_mutex);
	rc = __db_get_item_usagelist(tbl_name,p_usage_info,type);
	UNLOCK_EX(&db_mutex);
	return rc;
}


static int db_get_table_cb(void *param, int n_column, char ** column_value, char ** column_name)
{
	int ret = 0;
	int i =0;
	struct tbl_info *p_tbl_info = (struct tbl_info*) param;
	if(NULL == param)
	{
		printf("db_get_recoder_count_cb param setting is null!\n");
		return FAIL;
	}

	for(i = 0; i < n_column; ++i)
	{
	//printf("%d--%s:%s\n", p_tbl_info->cur_count, column_name[i], column_value[i]);
		if(0 == strcmp(column_name[i], "name")){

			if(p_tbl_info->cur_count < p_tbl_info->max_count)
			{
				strcpy(p_tbl_info->tbl_lst[p_tbl_info->cur_count].tbl_name,column_value[i]);//, DATETIME_STR_LEN);
				p_tbl_info->cur_count ++;
			}
		}
	}
	return NO_ERROR;
}
/*
 * get table list of database
 *
 */
int db_get_table(struct tbl_info *value, e_loder_sqlite type)
{
	int nRet = 0;
	char sql_cmd[256]={0};

	sprintf(sql_cmd, "select name from sqlite_master where type='table'");

	nRet = db_exec_cmd(sql_cmd, db_get_table_cb, value, type);
	if(0 != nRet )
	{
		printf("get table's row failed !\n");
		return FAIL;
	}

	return nRet;
}

int db_compare_item(char *tbl_name)
{
	struct item_info user_item_info, restore_item_info;
	struct usage_info restore_item_usage_info;
	char szSqlCmd[256]={0};
	int i, j, rc;

    if(0 == strcmp(tbl_name, "UsageHistory"))
    {
        memset(&restore_item_usage_info,0,sizeof(restore_item_usage_info));
		sprintf(szSqlCmd, "delete from %s", tbl_name);
	    rc = db_exec_cmd(szSqlCmd, NULL, NULL, E_LOADER_USER);
		rc = db_get_item_usagelist(tbl_name, &restore_item_usage_info, E_LOADER_RESTORE);

		for(i = 0; i < restore_item_usage_info.max_count; i++)
        {
            memset(szSqlCmd, 0, sizeof(szSqlCmd));
            sprintf(szSqlCmd, "INSERT INTO UsageHistory(StartTime, EndTime, RoamType, TxBytes, RxBytes) VALUES (%d, %d, %d, %llu, %llu)", \
				restore_item_usage_info.usage_lst[i].start_time, restore_item_usage_info.usage_lst[i].end_time, restore_item_usage_info.usage_lst[i].roam_type,
				restore_item_usage_info.usage_lst[i].tx_bytes, restore_item_usage_info.usage_lst[i].rx_bytes);
			rc = db_exec_cmd(szSqlCmd, NULL, NULL, E_LOADER_USER);
			if(rc)
			{
				OEM_LOG_INFO(OEM_LOG_ERROR, "update value failed!\n");
				return FAIL;
			}
        }

		//if(restore_item_usage_info.usage_lst != NULL)
		//{
		//	free(restore_item_usage_info.usage_lst);
		//}
		FREE(restore_item_usage_info.usage_lst);
    }
    else
    {
    	memset(&user_item_info,0,sizeof(user_item_info));
		memset(&restore_item_info,0,sizeof(restore_item_info));

		rc = db_get_item_list(tbl_name, &user_item_info, E_LOADER_USER);
		if(rc)
		{
		    OEM_LOG_INFO(OEM_LOG_ERROR, "item_list is NULL!\n");
			return 0;
		}
		rc = db_get_item_list(tbl_name, &restore_item_info, E_LOADER_RESTORE);

		for(i = 0; i < user_item_info.max_count; i++)
        {
   	       for(j = 0; j < restore_item_info.max_count; j++)
   	       {
			  if(!strcmp(user_item_info.item_lst[i].item_name, restore_item_info.item_lst[j].item_name))
			  {
				  rc = db_update_value(tbl_name,user_item_info.item_lst[i].item_name, restore_item_info.item_lst[j].item_value,user_item_info.item_lst[i].type);
				  if(rc)
				  {
						OEM_LOG_INFO(OEM_LOG_ERROR, "update value failed!\n");
						FREE(restore_item_info.item_lst);
						FREE(user_item_info.item_lst);
						return FAIL;
				  }
			  }
	       }
        }

		//if(user_item_info.item_lst != NULL || restore_item_info.item_lst != NULL)
		//{
		//	free(user_item_info.item_lst);
		//	free(restore_item_info.item_lst);
		//}
		FREE(user_item_info.item_lst);
		FREE(restore_item_info.item_lst);

    }

   return rc;

}


int db_restore(void)
{
	struct tbl_info user_tbl_info, restore_tbl_info;
	int i, j, rc = 0;

	rc = db_get_tabl_list(&user_tbl_info, E_LOADER_USER);
	rc = db_get_tabl_list(&restore_tbl_info, E_LOADER_RESTORE);


	for(i = 0; i < user_tbl_info.max_count; i++)
	{
		for(j = 0; j < restore_tbl_info.max_count; j++)
		{
			if(!strcmp(user_tbl_info.tbl_lst[i].tbl_name, restore_tbl_info.tbl_lst[j].tbl_name))
			{
				db_compare_item(user_tbl_info.tbl_lst[i].tbl_name);
			}
		}
	}

    FREE(user_tbl_info.tbl_lst);
	FREE(restore_tbl_info.tbl_lst);

	return rc;

}

#if 0
int db_restore_to_factory(void)
{
	int nRet = 0;//NO_ERROR;
	char szSqlCmd[128]={0};
	int column_count = 0;
	struct tbl_info p_tbl_info;
	int i = 0;

	sprintf(szSqlCmd, "select count(name) as column_count from sqlite_master where type='table';");
	nRet = db_exec_cmd(szSqlCmd, db_get_table_count_cb, &column_count);
	if(NO_ERROR != nRet)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "get table's row failed !\n");
		return FAIL;
	}

	if(0 >= column_count)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "get table count failed !\n");
		return FAIL;
	}
	else
	{
		//malloc memery from heap
		p_tbl_info.tbl_lst = calloc(column_count, sizeof(struct tbl_name));
		if(NULL == p_tbl_info.tbl_lst)
		{
			OEM_LOG_INFO(OEM_LOG_ERROR, "calloc memery failed !\n");
			return -1;
		}
		p_tbl_info.cur_count = 0;
		p_tbl_info.max_count = column_count;
	}

	nRet = db_get_table(&p_tbl_info);
	if(nRet != NO_ERROR)
	{
		OEM_LOG_INFO(OEM_LOG_ERROR, "get table list failed !\n");
		return NO_ERROR;
	}

	for(i = 0; i< p_tbl_info.cur_count; ++i)
	{
		if((strcmp("DevicePushInfo", p_tbl_info.tbl_lst[i].tbl_name) == 0)
				||(strcmp("UsageHistory", p_tbl_info.tbl_lst[i].tbl_name) == 0))
			sprintf(szSqlCmd, "delete from %s", p_tbl_info.tbl_lst[i].tbl_name);
		else
			sprintf(szSqlCmd, "update %s set value=factory_value ", p_tbl_info.tbl_lst[i].tbl_name);

		nRet = db_exec_cmd(szSqlCmd, NULL, NULL);
		if(nRet != NO_ERROR)
		{
			OEM_LOG_INFO(OEM_LOG_ERROR, "restore factore info failed !\n");
			OEM_LOG_INFO(OEM_LOG_ERROR, "sql_cmd:%s\n", szSqlCmd);

			break;
		}
	}
	return NO_ERROR;

}
#endif

int printf(char* sprint_buf, int size, char *fmt, ...)
{
  va_list args;
  int n;

  va_start(args, fmt);
  n = vsnprintf(sprint_buf, size, fmt, args);
  va_end(args);

  return n;
}


#ifdef SQLITE_PC_DEBUG
int main(int argc, char *argv[])
{
	usim_custom_t usim;
//	backup_db();
	int rc = query_db_init_struct(MODULE_USIM, &usim);
	printf("rc = %d \n", rc);//usim.sim_hot_swap);
	usim.sim_lock +=2;
	update_db_from_struct(MODULE_USIM, &usim, "SimLock");//USIM_TAG_SIM_LOCK);
	printf("rc = %d \n", rc);//usim.sim_hot_swap);

	printf("%d \n", usim.sim_hot_swap);
	printf("%d \n", usim.	sim_lock);
	printf("%d \n", usim.	auto_sim_lock);
	printf("%d \n", usim.	sim_unlock_mode);
	printf("%d \n", usim.	nck_unlock_times);
	printf("%d \n", usim.	rck_unlock_times);
	printf("%d \n", usim.	pin_lock);
	printf("%d \n", usim.	auto_unlock_pin);
	printf("%d \n", usim.	show_msisdn_mode);
	printf("%d \n", usim.	ussd_mode);
	return 0;
}
#endif
