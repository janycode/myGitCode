#ifndef __DB_COMMON_H__
#define __DB_COMMON_H__

typedef enum{
  /*secssion getSMSStoreState*/
  E_PARAM_U8 = 0,
  E_PARAM_BOOL,
  E_PARAM_U16,
  E_PARAM_INT,
  E_PARAM_U32,
  E_PARAM_U64,
  E_PARAM_I64,
  E_PARAM_STR,
  E_PARAM_ENUM,
  E_PARAM_NULL,
  E_PARAM_F32,
  E_PARAM_OBJECT, // Connie add for json parser, 2014/8/12
  E_PARAM_OBJECT_DATA,
  E_PARAM_ARRAY_DATA,
  E_PARAMTYPE_MAX,
}e_paramtype_type;

typedef struct param_key_match_struct
{
   void * param;
   char * key;
   e_paramtype_type param_type;
   int param_len;
   int param_id;
}param_key_info_t;

typedef struct param_key_table_info_struct
{
  char *table_name;
  param_key_info_t * param_table;
}param_table_info_t;


//extern int db_query_db_init_struct(char *tbl_name, param_key_match_list_t *param_lst);
//extern int db_update_db_from_struct(char *tbl_name, param_key_match_list_t *param_lst, int id);
extern int db_query_db_init_struct(param_table_info_t *tbl_info);
extern int db_update_db_from_struct(param_table_info_t *tbl_info, int id);
int db_get_value(char *tbl_name, char *key, void*value, e_paramtype_type type, int len);
int db_insert_record(char *tbl_name,  char *key, void*value, e_paramtype_type type);
int db_update_value(char *tbl_name, char *key, void*value, e_paramtype_type type);
int db_reload_value(param_table_info_t *tbl_info, int id) ;
int db_delete_record(char *tbl_name, char *key, void*value, e_paramtype_type type);
//extern int db_restore_db(char *src_db);
//extern int db_backup_db(char *dst_db);
extern int db_restore_to_factory(void);
extern int db_close_sqlite(void);
extern int db_restore(void);

#endif

