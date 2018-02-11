/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : run.c
  Version       : Initial Draft
  Author        : yuan.jiang
  Created       : 2018/1/17
  Last Modified :
  Description   : C code for baidu face detection.
  Function List :
              encode_img_to_base64
              get_access_token
              get_access_token_cb
              get_param_from_json_object
              parse_json_result
              print_result
              read_img_encode
              send_post
              send_post_cb
			  main
  History       :
  1.Date        : 2018/1/17
    Author      : yuan.jiang
    Modification: Created file
  2.Date         : 2018/2/6
    Author       : yuan.jiang
    Modification : add face match

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // close()
#include <strings.h> // bzero()
#include <errno.h>
//#include <curl/curl.h>

#include "base64.h"
#include "lib/curl/curl-7.29.0/include/curl/curl.h"  	/* libcurl.so */
#include "lib/json/json-c-0.11/json.h"  				/* libjson-c.so */

#define AK                  		"8WRsAAo2AWGUtuT4GI1LZwbz"
#define SK                  		"aIUxawGasEKlF3inR7PtedVnENYGGOTD"
#define ACCESS_TOKEN_URL  			"https://aip.baidubce.com/oauth/2.0/token"
#define FACE_DETECT_POST_PATH    "https://aip.baidubce.com/rest/2.0/face/v2/detect"
#define FACE_MATCH_POST_PATH     "https://aip.baidubce.com/rest/2.0/face/v2/match"
#define POST_FIELDS        			"max_face_num=5&face_fields=age,beauty,expression,faceshape,gender,glasses,landmark,race,qualities&image="

#define BUFF_SIZE 		(2*1024*2014)   /* baidu API 文档要求每张图片不大于2M */
#define RESULT_SIZE 	(1024)
#define WRITE_SIZE   	(4096)
#define BASE64_FILE 	"./encode/base64_info_for_post.txt"

#define MALLOC(ptr,sz) \
	do{  \
		(ptr)=malloc((sz));  \
		if(NULL!=(ptr))  \
			memset((ptr),0,(sz)); \
	}while(0)

#define FREE(ptr) \
	do{  \
		if(NULL!=(ptr)) \
			free((ptr));  \
		(ptr) = NULL; \
	}while(0)

typedef struct http_detect_response_body {
	int result_num;				//人脸数目
	int loc_left;				//左边界
	int loc_top;				//上边界
	int loc_width;				//宽度
	int loc_height;				//高度
	int face_probability;		//人脸置信度
	int age;					//年龄
	double round;				//圆形脸
	char gender[16];			//性别
	char race[16];				//人种
	double beauty;				//美丑分
	int expression;				//表情
	int glasses;				//眼镜
	double blur;				//模糊度
} http_detect_response_body_t;
typedef struct http_match_response_body {
	double score;					//对比得分，超过80分则认为是同一张脸
} http_match_response_body_t;

typedef struct image_data{
	char *img_buff;
	char *base64_buff;
	char *base64_buff2;
} img_data_t;

//char img_buff[BUFF_SIZE] = {0};
//char base64_buff[BUFF_SIZE] = {0};
char *p_json_result = NULL;
static http_detect_response_body_t detect_res_body = {0};
static http_match_response_body_t match_res_body = {0};
static char *detect_param[] = {
	"result_num",		/* 0  */
	"result",			/* 1  */
	"location",			/* 2  */
	"left",				/* 3  */
	"top",				/* 4  */
	"width",			/* 5  */
	"height",			/* 6  */
	"face_probability",	/* 7  */
	"age",				/* 8  */
	"gender",			/* 9  */
	"race",				/* 10 */
	"beauty",			/* 11 */
	"expression",		/* 12 */
	"glasses",			/* 13 */
};
static char *match_param[] = {
	"result",			/* 0 */
	"score",			/* 1 */
};
typedef enum FACE_MODE
{
	INVALID_FACE_MODE = 0,
    FACE_DETECT_MODE,
	FACE_MATCH_MODE,
}FACE_MODE_ENUM;

static size_t send_post_cb(void *ptr, size_t size, size_t nmemb, void *stream)
{
    char *result_body = (char *)ptr;
#if 0
    // 获取到的body存放在ptr中，先将其转换为string格式
    detect_result = std::string((char *) ptr, size * nmemb);
#endif

#if 0
	/* 仅能用于单张图片的人脸识别 */
    /* 可以用realloc的方式申请内存操作，传递给全局指针 */
    static int len = 0;

    p_json_result = (char *)realloc (p_json_result, size * nmemb + len);
    strncpy (p_json_result + len, result_body, size * nmemb);
    len += size * nmemb;
    //printf ("\n\n>> p_json_result = %s\n\n", p_json_result);
#endif
	/* 可用于多张图片的人脸识别，不冲突 */
	int *p_len = (int *)stream;

    p_json_result = (char *)realloc (p_json_result, size * nmemb + (*p_len));
	/* http返回的数据的长度为不定长 */
    strncpy (p_json_result + (*p_len), result_body, size * nmemb);
    *p_len += size * nmemb;

	//printf ("\n>> p_len = %d\n\n", *p_len);
    printf ("\n\n>> p_json_result = %s\n\n", p_json_result);

    return size * nmemb;
}

int send_post(FACE_MODE_ENUM face_mode, char* access_token, char *base64_buff)
{
    CURL *curl;
    CURLcode result_code;
    int is_success;
    char post_url[256] = {0};
    char token_str[32] = "?access_token=";

	if (access_token == NULL) {
		printf ("accses_token is empty...\n");
	}

	switch (face_mode)
	{
	    case FACE_DETECT_MODE:
			strncpy (post_url, FACE_DETECT_POST_PATH, strlen(FACE_DETECT_POST_PATH));
	        break;
	    case FACE_MATCH_MODE:
			strncpy (post_url, FACE_MATCH_POST_PATH, strlen(FACE_MATCH_POST_PATH));
	        break;
	    default:
	        printf ("ERROR: No this face mode. face_mode = %d\n", face_mode);
			break;
	}
    strncat (post_url, token_str, strlen(token_str));
    strncat (post_url, access_token, strlen(access_token));
    printf (">> Post request url = %s\n", post_url);

    curl = curl_easy_init();
    if (curl) {
		/* 进行多张人脸的单张图片的人脸识别时，每次长度会重新置0 */
		int response_len = 0;
        struct curl_httppost *post = NULL; // 百度给的demo类型有误，curl库中类型为 struct curl_httppost
        struct curl_httppost *last = NULL; // 百度给的demo类型有误，curl库中类型为 struct curl_httppost
        curl_easy_setopt(curl, CURLOPT_URL, post_url);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
		switch (face_mode)
		{
		    case FACE_DETECT_MODE:
				curl_formadd(&post, &last, CURLFORM_COPYNAME, "max_face_num", CURLFORM_COPYCONTENTS, "5", CURLFORM_END);
				curl_formadd(&post, &last, CURLFORM_COPYNAME, "face_fields", CURLFORM_COPYCONTENTS, "age,beauty,expression,faceshape,gender,glasses,landmark,race,qualities", CURLFORM_END);
				curl_formadd(&post, &last, CURLFORM_COPYNAME, "image", CURLFORM_COPYCONTENTS, base64_buff, CURLFORM_END);
		        break;
		    case FACE_MATCH_MODE:
				//printf (">> test: strlen(base64_buff)=%ld\n", strlen(base64_buff));
				curl_formadd(&post, &last, CURLFORM_COPYNAME, "images", CURLFORM_COPYCONTENTS, base64_buff, CURLFORM_END);
		        break;
		    default:
		        printf ("ERROR: No this face mode. face_mode = %d\n", face_mode);
				break;
		}
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_len);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, send_post_cb);
        result_code = curl_easy_perform(curl);
        if (result_code != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result_code));
            is_success = 1;
            return is_success;
        }
        //strncpy (json_result, detect_result, sizeof (detect_result));
        //strncpy (json_result, p_json_result, strlen(p_json_result));

        //printf (">> response_len = %d\n", response_len);
        //printf (">> p_json_result = %s\n", p_json_result);

        curl_easy_cleanup(curl);
        is_success = 0;
    } else {
        fprintf(stderr, "curl_easy_init() failed.");
        is_success = 1;
    }

    return is_success;
}

int merge_base64_buff(char **p, char *buff1, char *buff2)
{
	int buff_len = 0;
	char delim[2] = ",";

	if (NULL == buff1 || NULL == buff2) {
		printf ("buff1 or buff2 is empty.\n");
		return -1;
	}
	buff_len = strlen (buff1) + strlen (buff2) + 2; // ',' and '\0'
	*p = (char *)malloc (buff_len);
	memset (*p, 0, buff_len);
	strncpy (*p, buff1, strlen (buff1));
	strncat (*p, delim, strlen (delim));
	strncat (*p, buff2, strlen (buff2));
	printf (">> strlen (*p) = %ld\n", strlen (*p));

	/* for cheek base64 info. */
#if 1
	int fd = 0;
	int ops = 0;
	int size = 0;
	int len = WRITE_SIZE;
	int length = strlen (*p);

	fd = open(BASE64_FILE, O_CREAT | O_RDWR, 0664);
    if (fd < 0) {
        printf (">> Create base64_file file failed... fd=%d\n %s\n", fd, strerror(errno));
        return -1;
    }
    while (length) {
        if(length < len)
            len = length;
        size = write (fd, *p + ops, len);
        ops += size;
        length -= size;
    }
    close (fd);
#endif
	return 0;
}


int encode_img_to_base64 (char *p_img, int cnt, char *img_buff, char *base64_buff)
{
    int len = WRITE_SIZE;

    base64_encode(base64_buff, img_buff, cnt, BUFF_SIZE);

    /* for cheek base64 info. */
#if 1
    int fd = 0;
    int size = 0;
    int ops = 0;
    int length = strlen (base64_buff);
	char base64_file[64] = {0};
	char img_name[32] = {0};
	char *file_name = NULL;
	char *file_format = NULL;

	/* Cut image file name from the argv[] array. */
	strcpy(img_name, (file_name=strrchr(p_img, '/')) ? file_name+1 : p_img);  	  // A01.jpg
	file_name = strtok (img_name, "."); 											// A01
	file_format = strtok (NULL, ".");												// jpg
	strncpy (base64_file, "./encode/", strlen("./encode/")); 						// ./encode/
	strncat (base64_file, file_name, strlen (file_name)); 							// ./encode/A01
	strncat (base64_file, "_base64.txt", strlen ("_base64.txt"));					// ./encode/A01_base64.txt
	printf (">> base64_file = %s\n", base64_file);

    //fd = open (BASE64_FILE, O_CREAT | O_RDWR, 0664);
    fd = open (base64_file, O_CREAT | O_RDWR, 0664);
    if (fd < 0) {
        printf (">> Create base64_file file failed... fd=%d\n %s\n", fd, strerror(errno));
        return -1;
    }
    while (length) {
        if(length < len)
            len = length;
        size = write (fd, base64_buff + ops, len);
        ops += size;
        length -= size;
    }
    close (fd);
#else
    FILE *fp = NULL;
    fp = fopen (BASE64_FILE, "w+");
    size = fwrite (base64_buff, sizeof (char), length, fp);
    if (size != length)
        printf ("fwrite error...\n");

    fclose(fp);
#endif
  
    return 0;
}

int read_img_encode (char *p_img, char **img_buff, char **base64_buff)
{
    int fd = 0;
    int size = 0;
    int ops = 0;
    int length = 0;

    fd = open(p_img, O_RDONLY);
    if (fd < 0) {
        printf (">> Open error...");
        return -1;
    }

	*img_buff = (char *)malloc (BUFF_SIZE);
	*base64_buff = (char *)malloc (BUFF_SIZE);

    length = BUFF_SIZE - 1;
    bzero (*img_buff, BUFF_SIZE);
    while ((size = read(fd, *img_buff + ops, length)) > 0) {
        ops += size;
        length -= size;
    }
    printf (">> read img bytes = %d\n", ops);
	printf (">> image name = %s\n", p_img);
    encode_img_to_base64(p_img, ops, *img_buff, *base64_buff);
    
    close(fd);
    return 0;
}

static size_t get_access_token_cb (void *ptr, size_t size, size_t nmemb, void *stream)
{
#if 0
    // Baidu C++ content.
    // 获取到的body存放在ptr中，先将其转换为string格式
    std::string s((char *) ptr, size * nmemb);
    // 开始获取json中的access token项目
    Json::Reader reader;
    Json::Value root;
    // 使用boost库解析json
    reader.parse(s,root);
    std::string* access_token_result = static_cast<std::string*>(stream);
    *access_token_result = root["access_token"].asString();
#endif
    /* ----------------------- */
    char *p_body = (char *)ptr;
    char response_body[RESULT_SIZE] = {0};
    char *access_token_result = (char *)stream;
    char *p = NULL;
    //char *tmp = NULL;
    int i = 0;
    char sub_string[2][128] = {0};

    strncpy (response_body, p_body, size*nmemb);
    printf (">> Response_body = %s\n", response_body);

    /* Parse access token. */
    // access_token":"24.00cd178deb1c554b3c806f9aa9727199.2592000.1518849794.282335-10701883",......
    if ((p = strstr (response_body, "access_token"))) {
        printf (">> Get access_token success...\n");

        p = strtok (p, ",");
        //printf (">> p1 = %s\n", p);
        p = strtok (p, "\"");
        //printf (">> p2 = %s\n", p); // access_token

        while (p = strtok (0, "\"")) {
            memcpy (sub_string[i], p, strlen (p));
            //printf (">> sub_string[%d] = %s\n", i, sub_string[i]); // [0] : [1] 24.00cd178deb1c554b3c806f9aa9727199.2592000.1518849794.282335-10701883
            i++;
        }
    }
    strncpy (access_token_result, sub_string[1], strlen(sub_string[1]));
    
    return size*nmemb;
}

int get_access_token (char **access_token)
{
    CURL *curl;
    CURLcode result_code;
    int error_code = 0;
    char url[256] = {0};
    char grant_type_str[64] = "?grant_type=client_credentials";
    char client_id_str[64] = "&client_id=";
    char client_secret_str[64] = "&client_secret=";

    memset (url, 0, sizeof (url));
    /* https://aip.baidubce.com/oauth/2.0/token  ?grant_type=client_credentials  &client_id=  ${api_key}  &client_secret=  ${secret_key}
     * (1)                                        (2)                            (3)          (4)         (5)              (6)        */
    strncpy (url, ACCESS_TOKEN_URL, strlen (ACCESS_TOKEN_URL));      /* (1) */
    strncat (url, grant_type_str, strlen (grant_type_str));          /* (2) */
    strncat (url, client_id_str, strlen (client_id_str));            /* (3) */
    strncat (url, AK, strlen (AK));                                  /* (4) */
    strncat (url, client_secret_str, strlen (client_secret_str));    /* (5) */
    strncat (url, SK, strlen (SK));                                  /* (6) */
    printf (">> Get access_token url = %s\n", url);

    curl = curl_easy_init();
    if (curl) {
		char access_token_result[128] = {0};
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, access_token_result);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_access_token_cb);
        result_code = curl_easy_perform(curl);
        if (result_code != CURLE_OK) {
            fprintf(stderr, ">> curl_easy_perform() failed: %s\n", curl_easy_strerror(result_code));
            return -1;
        }

		int res_len = sizeof (access_token_result);
		*access_token = (char *)malloc(res_len);
        strncpy (*access_token, access_token_result, res_len);
        printf (">> access_token = %s\n", *access_token);

        curl_easy_cleanup(curl);
        error_code = 0;
    } else {
        fprintf(stderr, ">> curl_easy_init() failed.");
        error_code = 1;
    }

    return error_code;
}

#if 0
// 这个接口，包含了很多数据类型(array, int, string, object)。组成json包，socket传输，为上层提供数据，便于展示。  
void make_a_common_json_string(void)
{
    printf("####################################\n");
    int i=0;
    struct json_object *obj_all  = json_object_new_object();
    struct json_object *obj_info = json_object_new_object();
    struct json_object *obj_work = json_object_new_object();
    struct json_object *obj_life = json_object_new_object();
    struct json_object *arr_month_grade = json_object_new_array();

    for(i=1; i<13; i++){
        struct json_object *obj_int = json_object_new_int(i);
        json_object_array_add(arr_month_grade, obj_int);
    }

    //const char *str = "{\"职位\": \"programmer\", \"部门\": \"研发\", \"addres\": \"BJ.3\"}";
    //json_object *obj_str = json_tokener_parse(str);

    json_object_object_add(obj_work, "position", json_object_new_string("programmer"));
    json_object_object_add(obj_work, "department", json_object_new_string("make code"));
    json_object_object_add(obj_work, "address", json_object_new_string("BJ.3.103"));
    json_object_object_add(obj_work, "mounth_grade", arr_month_grade);

    json_object_object_add(obj_info, "Work", obj_work);

    json_object_object_add(obj_life, "Name", json_object_new_string("DS.wen"));
    json_object_object_add(obj_life, "Age", json_object_new_int(24.34));
    json_object_object_add(obj_life, "M-O", json_object_new_string("Male"));

    json_object_object_add(obj_info, "Life", obj_life);

    json_object_object_add(obj_all,"W.DS_Info", obj_info);
    printf("all_string:%s\n", json_object_to_json_string(obj_all));

    json_object_put(obj_all);
    return ;
}
#endif

int get_param_from_json_object
(
        json_object * req_object,
        const char  * param_name,
        void ** param_value   /* out */
)
{
    json_object * object_this_param = NULL;

    if (!req_object || !param_value)
        return -1;

    object_this_param = json_object_object_get(req_object, param_name);
    if (object_this_param == NULL)
    {
        printf ("Can't get object for param %s\n", param_name);
        return -1;
    }

	//printf (">> type() = %d\n", json_object_get_type(object_this_param));
    switch (json_object_get_type(object_this_param))
    {
	    case json_type_int:
	    {
	        *(int*)param_value = json_object_get_int(object_this_param);
	        break;
	    }
		case json_type_double:
	    {
	        *(double*)param_value = json_object_get_double(object_this_param);
	        break;
	    }
		/* New define on the json head file. */
	    case json_type_int64:
	    {
	        *(int64_t*)param_value = json_object_get_int64(object_this_param);
	        break;
	    }
	    case json_type_string:
	    {
	        *param_value = (void*)json_object_get_string(object_this_param);
	        break;
	    }
	    case json_type_object:
	    {
			void *tmp = (void*)json_object_get_string(object_this_param);
			strncpy ((char *)param_value, (char *)tmp, strlen (tmp));
			/* int i = 0;
			for(i = 0; i < strlen(tmp); i++)
				*((char *)param_value + i) = *((char *)tmp + i); */
	        break;
	    }
	    case json_type_array:
	    {
	        int i;
	        char *tmp_val = NULL;
	        const char *arr_elem = NULL;
			int arr_elem_len = 0;
	        int array_len = 0;

	        array_len = json_object_array_length(object_this_param);
			//printf ("array_len = %d\n", array_len);//1

	        for (i = 0; i < array_len - 1; i++)
	        {
	                struct json_object *obj = json_object_array_get_idx(object_this_param, i);
	                arr_elem = (const char*)json_object_to_json_string(obj);
					arr_elem_len = strlen (arr_elem);
					tmp_val = (char *) malloc (arr_elem_len);
	                //printf("arr_elem: %s\n", arr_elem);
	                strcat(tmp_val, arr_elem);
	                strcat(tmp_val, ", ");
	        }
	        json_object *obj = json_object_array_get_idx(object_this_param, i);
	        arr_elem = (const char*)json_object_to_json_string(obj);
			//arr_elem_len = json_object_get_string_len(obj);
			arr_elem_len = strlen (arr_elem);

	        //printf (">> *arr_elem = %s arr_elem_len = %d\n\n", arr_elem, arr_elem_len);
			tmp_val = (char *) malloc (arr_elem_len + 1);
			memset (tmp_val, 0, arr_elem_len + 1);
	        strncpy(tmp_val, arr_elem, arr_elem_len);
	        *param_value =   tmp_val;
	        break;
	    }
	    default:
	    {
	        printf (">> invalid param_type: %d\n", json_object_get_type(object_this_param));
	        return -1;
	    }
    }
	//printf (">> *param_value = %s\n", (char *)*param_value);

    return 0;
}


void print_result (char *p)
{
	char tmp[4096] = {0};
	strncpy (tmp, p, sizeof(tmp));
	printf ("Test tmp = %s\n", tmp);
}

int parse_match_json_result (char *p_json_result)
{
	printf ("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	printf (">> Start parse face match result(Only for human face):\n");

    struct json_object *my_obj = NULL;
    struct json_object *ret_obj = NULL;
	int rc = 0;
	char *p_value = NULL;
    const char *p_str = p_json_result;  /* json格式的字符串 */
	int score = 0;

    /* 将符合json格式的字符串构造为一个json对象 */
    my_obj = json_tokener_parse(p_str);
    /* 在对象里用key值去找，获取它的value值 */
	get_param_from_json_object(my_obj, match_param[0], (void**)&p_value); /* result */
	//printf (">> p_value: %s\n", p_value);
	/* 将子json数据转化为json对象 */
	ret_obj = json_tokener_parse(p_value);
	/* 从该json对象中获取value的值 */
	get_param_from_json_object(ret_obj, match_param[1], (void*)&match_res_body.score); /* score>80 match; score<80 not match. */
	printf (">> Faces match score = %d\n", (int)match_res_body.score);

	score = (int)match_res_body.score;
	if (score >= 80) {
		printf ("^^^\n\tSuccess: Score >= 80, It's the same people.\n^^^\n");
	} else {
		printf ("^^^\n\tFail: Score < 80, It isn't the same people.\n^^^\n");
	}
}


int parse_detect_json_result (char *p_json_result)
{
	printf ("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	printf (">> Start parse face detect result(Only for one picture):\n");

    struct json_object *my_obj = NULL;
    struct json_object *ret_obj = NULL;
	int rc = 0;
	char *p_value = NULL;
    const char *p_str = p_json_result;  /* json格式的字符串 */

    /* 将符合json格式的字符串构造为一个json对象 */
    my_obj = json_tokener_parse(p_str);
    printf(">> 1st-tokener parse: %s\n", json_object_to_json_string(my_obj));

    /* 在对象里用key值去找，获取它的value值 */
    ret_obj = json_object_object_get(my_obj, detect_param[0]);
	detect_res_body.result_num = atoi(json_object_to_json_string(ret_obj));
	printf ("Detect face num: %d\n", detect_res_body.result_num);
    //json_object_put(ret_obj); /* 对应get使用完释放内存 */

	/* 在对象里用key值去找，然后删除它的key:value */
    //json_object_object_del(my_obj, "result_num");
	//printf(">> 2nd-tokener parse: %s\n", json_object_to_json_string(my_obj));

    //ret_obj = json_object_object_get(my_obj, "result");
    //printf(">> obj_get result = %s\n", json_object_to_json_string(ret_obj));
	//json_object_put(ret_obj); // 还没使用完，不能put
#if 0
	p_str = json_object_to_json_string(ret_obj);
	my_obj = json_tokener_parse(p_str);

	ret_obj = json_object_object_get(my_obj, "result");
	printf(">> obj_get location = %s\n", json_object_to_json_string(ret_obj));
#endif
	get_param_from_json_object(my_obj, detect_param[1], (void**)&p_value); /* result */
	//print_result(p_value);
	ret_obj = json_tokener_parse(p_value);

	struct json_object *location_obj = NULL;
	location_obj = json_tokener_parse(p_value);
	get_param_from_json_object(location_obj, detect_param[2], (void**)&p_value); /* location */
	//print_result(p_value);

	location_obj = json_tokener_parse(p_value);
	get_param_from_json_object(location_obj, detect_param[3], (void*)&detect_res_body.loc_left); /* left */
	get_param_from_json_object(location_obj, detect_param[4], (void*)&detect_res_body.loc_top); /* top */
	get_param_from_json_object(location_obj, detect_param[5], (void*)&detect_res_body.loc_width); /* width */
	get_param_from_json_object(location_obj, detect_param[6], (void*)&detect_res_body.loc_height); /* height */
	//printf ("left=%d, top=%d, width=%d, height=%d\n", detect_res_body.loc_left, detect_res_body.loc_top, detect_res_body.loc_width, detect_res_body.loc_height);
	printf ("face coord:\n\tleft_up (%d, %d) \t right_up (%d, %d)\n\tleft_down (%d, %d) \t right_down (%d, %d)\n",
			detect_res_body.loc_left, detect_res_body.loc_top,
			detect_res_body.loc_left + detect_res_body.loc_width, detect_res_body.loc_top,
			detect_res_body.loc_left, detect_res_body.loc_top + detect_res_body.loc_height,
			detect_res_body.loc_left + detect_res_body.loc_width, detect_res_body.loc_top + detect_res_body.loc_height);


	get_param_from_json_object(ret_obj, detect_param[7], (void*)&detect_res_body.face_probability); /* face_probability */
	printf ("face_probability = %d\n", detect_res_body.face_probability);
	get_param_from_json_object(ret_obj, detect_param[8], (void*)&detect_res_body.age); /* age */
	printf ("age = %d\n", detect_res_body.age);
	get_param_from_json_object(ret_obj, detect_param[9], (void**)&detect_res_body.gender); /* gender */
	//get_param_from_json_object(ret_obj, detect_param[9], (char *)&detect_res_body.round + 8); /* gender: it's also OK. */
	printf ("gender = %s\n", detect_res_body.gender);
	get_param_from_json_object(ret_obj, detect_param[10], (void**)&detect_res_body.race); /* race */
	printf ("race = %s\n", detect_res_body.race);
	get_param_from_json_object(ret_obj, detect_param[11], (void*)&detect_res_body.beauty); /* race */
	printf ("beauty = %lf\n", detect_res_body.beauty);
	get_param_from_json_object(ret_obj, detect_param[12], (void*)&detect_res_body.expression); /* race */
	printf ("expression = %d\n", detect_res_body.expression);
	get_param_from_json_object(ret_obj, detect_param[13], (void*)&detect_res_body.glasses); /* race */
	printf ("glasses = %d\n", detect_res_body.glasses);

	json_object_put(ret_obj); /* 对应get使用完释放内存 */
#if  0
    // 获取不同的json数据类型，int.double.string.array.object.
    struct json_object *val = json_object_new_int(18542);
    // 将key:value加入到对象my_obj中.
    json_object_object_add(my_obj, "wds", val);
    printf(">> 3th-object add: %s\n", json_object_to_json_string(my_obj));
    // 看function详解1
    json_object_put(val);

    printf(">> get string: %s\n", json_object_get_string(my_obj));
    printf(">> get lh: %d\n", json_object_get_object(my_obj));
    printf(">> get int: %d\n", json_object_get_int(my_obj));

    //make_a_common_json_string(); // 组json包
      
    // 看function详解1
    json_object_put(my_obj);
#endif
	if(p_value)
		free (p_value);
    return 0;
}

int main (int argc, char *argv[])
{
    char *access_token = NULL;
	img_data_t img_data = {0};

	printf ("argc = %d\n", argc);
	//face detect
	if (argc == 2) {

		printf ("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\nFace detect start...\n");
	    //1. encode image. >> base64_info
	    read_img_encode(argv[1], &img_data.img_buff, &img_data.base64_buff);
	    //2. Get access token. >> access_token
	    get_access_token(&access_token);
	    //3. send_post.
	    send_post(FACE_DETECT_MODE, access_token, img_data.base64_buff);
	    //4. parse result.
	    parse_detect_json_result (p_json_result);

		FREE (access_token);
		FREE (img_data.img_buff);
		FREE (img_data.base64_buff);
		FREE (p_json_result);
	}
	//face match
	else if(argc == 3) {
		printf ("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\nFace match start...\n");
		//1. encode image. >> base64_info
		read_img_encode(argv[1], &img_data.img_buff, &img_data.base64_buff);
		read_img_encode(argv[2], &img_data.img_buff, &img_data.base64_buff2);
		//2. Get access token. >> access_token
		get_access_token(&access_token);
	    //3. send_post.
	    char *two_img_base64_buff = NULL;
	    merge_base64_buff(&two_img_base64_buff, img_data.base64_buff, img_data.base64_buff2);
	    send_post(FACE_MATCH_MODE, access_token, two_img_base64_buff);
		//4. parse result.
		parse_match_json_result(p_json_result);

		FREE (access_token);
		FREE (img_data.img_buff);
		FREE (img_data.base64_buff);
		FREE (img_data.base64_buff2);
		FREE (two_img_base64_buff);
		FREE (p_json_result);
	}
	else {
		if (argc < 2 | argc > 3) {
			printf ("Please input image file, like this:\n"); 
			printf ("    1.face detect $: ./run ./image/0.jpg\n");
			printf ("    2.face match  $: ./run ./image/A.jpg ./image/B.jpg\n");
			return -1;
		}
	}
	
	return 0;
}
