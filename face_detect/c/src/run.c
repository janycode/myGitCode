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

#define AK                  "8WRsAAo2AWGUtuT4GI1LZwbz"
#define SK                  "aIUxawGasEKlF3inR7PtedVnENYGGOTD"
#define ACCESS_TOKEN_URL  "https://aip.baidubce.com/oauth/2.0/token"
#define POST_PATH          "https://aip.baidubce.com/rest/2.0/face/v2/detect"
#define POST_FIELDS        "max_face_num=5&face_fields=age,beauty,expression,faceshape,gender,glasses,landmark,race,qualities&image="

#define BUFF_SIZE 		(2*1024*2014)   /* baidu API 文档要求不大于2M */
#define RESULT_SIZE 	(1024)
#define WRITE_SIZE   	(4096)
#define BASE64_FILE 	"./encode/base64_info.txt"

typedef struct http_response_body {
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
} http_response_body_t;

//char img_buff[BUFF_SIZE] = {0};
//char base64_buff[BUFF_SIZE] = {0};
char *p_json_result = NULL;
static http_response_body_t http_res_body = {0};
static char *result_param[] = {
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
    //printf ("\n\n>> p_json_result = %s\n\n", p_json_result);

    return size * nmemb;
}

int send_post(char* access_token, char *base64_buff)
{
    CURL *curl;
    CURLcode result_code;
    int is_success;
    char post_url[256] = {0};
    char token_str[32] = "?access_token=";

    strncpy (post_url, POST_PATH, strlen(POST_PATH));
    strncat (post_url, token_str, strlen(token_str));
    strncat (post_url, access_token, strlen(access_token));
    printf (">> Post request url = %s\n", post_url);

    curl = curl_easy_init();
    if (curl) {
		/* 进行多张图片的人脸识别时，每次长度会重新置0 */
		int response_len = 0;
        struct curl_httppost *post = NULL; // 百度给的demo类型有误，curl库中类型为 struct curl_httppost
        struct curl_httppost *last = NULL; // 百度给的demo类型有误，curl库中类型为 struct curl_httppost
        curl_easy_setopt(curl, CURLOPT_URL, post_url);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "max_face_num", CURLFORM_COPYCONTENTS, "5", CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "face_fields", CURLFORM_COPYCONTENTS, "age,beauty,expression,faceshape,gender,glasses,landmark,race,qualities", CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "image", CURLFORM_COPYCONTENTS, base64_buff, CURLFORM_END);
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

int encode_img_to_base64 (int cnt, char *img_buff, char *base64_buff)
{
    int len = WRITE_SIZE;

    base64_encode(base64_buff, img_buff, cnt, BUFF_SIZE);

    /* for cheek base64 info. */
#if 1
    int fd = 0;
    int size = 0;
    int ops = 0;
    int length = strlen (base64_buff);

    fd = open (BASE64_FILE, O_CREAT | O_RDWR, 0664);
    if (fd < 0) {
        printf (">> Create base64_info file failed... fd=%d\n %s\n", fd, strerror(errno));
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
    encode_img_to_base64(ops, *img_buff, *base64_buff);
    
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

int parse_json_result (char *p_json_result)
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
    //printf(">> 1st-tokener parse: %s\n", json_object_to_json_string(my_obj));

    /* 在对象里用key值去找，获取它的value值 */
    ret_obj = json_object_object_get(my_obj, result_param[0]);
	http_res_body.result_num = atoi(json_object_to_json_string(ret_obj));
	printf ("Detect face num: %d\n", http_res_body.result_num);
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
	get_param_from_json_object(my_obj, result_param[1], (void**)&p_value); /* result */
	//print_result(p_value);
	ret_obj = json_tokener_parse(p_value);

	struct json_object *location_obj = NULL;
	location_obj = json_tokener_parse(p_value);
	get_param_from_json_object(location_obj, result_param[2], (void**)&p_value); /* location */
	//print_result(p_value);

	location_obj = json_tokener_parse(p_value);
	get_param_from_json_object(location_obj, result_param[3], (void*)&http_res_body.loc_left); /* left */
	get_param_from_json_object(location_obj, result_param[4], (void*)&http_res_body.loc_top); /* top */
	get_param_from_json_object(location_obj, result_param[5], (void*)&http_res_body.loc_width); /* width */
	get_param_from_json_object(location_obj, result_param[6], (void*)&http_res_body.loc_height); /* height */
	//printf ("left=%d, top=%d, width=%d, height=%d\n", http_res_body.loc_left, http_res_body.loc_top, http_res_body.loc_width, http_res_body.loc_height);
	printf ("face coord:\n\tleft_up (%d, %d) \t right_up (%d, %d)\n\tleft_down (%d, %d) \t right_down (%d, %d)\n",
			http_res_body.loc_left, http_res_body.loc_top,
			http_res_body.loc_left + http_res_body.loc_width, http_res_body.loc_top,
			http_res_body.loc_left, http_res_body.loc_top + http_res_body.loc_height,
			http_res_body.loc_left + http_res_body.loc_width, http_res_body.loc_top + http_res_body.loc_height);


	get_param_from_json_object(ret_obj, result_param[7], (void*)&http_res_body.face_probability); /* face_probability */
	printf ("face_probability = %d\n", http_res_body.face_probability);
	get_param_from_json_object(ret_obj, result_param[8], (void*)&http_res_body.age); /* age */
	printf ("age = %d\n", http_res_body.age);
	get_param_from_json_object(ret_obj, result_param[9], (void**)&http_res_body.gender); /* gender */
	//get_param_from_json_object(ret_obj, result_param[9], (char *)&http_res_body.round + 8); /* gender: it's also OK. */
	printf ("gender = %s\n", http_res_body.gender);
	get_param_from_json_object(ret_obj, result_param[10], (void**)&http_res_body.race); /* race */
	printf ("race = %s\n", http_res_body.race);
	get_param_from_json_object(ret_obj, result_param[11], (void*)&http_res_body.beauty); /* race */
	printf ("beauty = %lf\n", http_res_body.beauty);
	get_param_from_json_object(ret_obj, result_param[12], (void*)&http_res_body.expression); /* race */
	printf ("expression = %d\n", http_res_body.expression);
	get_param_from_json_object(ret_obj, result_param[13], (void*)&http_res_body.glasses); /* race */
	printf ("glasses = %d\n", http_res_body.glasses);

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
	char *p = argv[1];
    char *access_token = NULL;
	char *img_buff = NULL;
	char *base64_buff = NULL;
    
	printf ("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\nFace detect start...\n");
    //1. encode image. >> base64_info
    read_img_encode(p, &img_buff, &base64_buff);
    //2. Get access token. >> access_token
    get_access_token(&access_token);
    //3. send_post.
    send_post(access_token, base64_buff);//http struct
    //4. parse result.
    parse_json_result(p_json_result);

	if(access_token)
		free (access_token);
	if(img_buff)
		free (img_buff);
	if(base64_buff)
		free (base64_buff);
	if(p_json_result)
    	free (p_json_result);
	return 0;
}
