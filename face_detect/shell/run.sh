#!/bin/bash
# Web path: https://console.bce.baidu.com/ai/
# Test type: 人脸识别 >> 人脸检测
# Application: jan_face_detection
# AppID: 10701883
# Author: jiangyuan

current_path=${PWD}
#echo ${current_path}

# Baidu Key Info #
face_detection_token_path="https://aip.baidubce.com/oauth/2.0/token"
face_detection_api_key="8WRsAAo2AWGUtuT4GI1LZwbz"
face_detection_secret_key="aIUxawGasEKlF3inR7PtedVnENYGGOTD"

# Baidu POST path #
face_detection_post_path="https://aip.baidubce.com/rest/2.0/face/v2/detect"

# Internal paramter #
max_face_num=50
face_fields=age,beauty,expression,faceshape,gender,glasses,landmark,race,qualities

#curl -i -k 'https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=${face_detection_api_key}&client_secret=${face_detection_secret_key}'

function base64_image() 
{
	base64_file=${current_path}"/encode/base64_res.txt"

	# Start encode image to base64.
	base64 $1 > ${base64_file}
}

function get_access_token() 
{
#	echo "---------------------------------"
	cmd="curl -s -k ${face_detection_token_path}?grant_type=client_credentials&client_id=${face_detection_api_key}&client_secret=${face_detection_secret_key}"
#	echo "cmd: ${cmd}"
	access_token_response=`$cmd`
#	echo "access_token_response: ${access_token_response}"
	access_token=`echo ${access_token_response} | awk -F ':' '{print $2}' | cut -d ',' -f 1 | sed 's/\"//g'` 
	echo "access_token: ${access_token}"
	echo "Get access_token success..."
}

function send_post_request() 
{
	post_request_path=${face_detection_post_path}"?access_token="${access_token}
#	echo "post_request_path: ${post_request_path}"
	
	base64_data=`cat ${base64_file}`
	
	post_param="max_face_num="${max_face_num}"&face_fields="${face_fields}
	post_data="image="${base64_data}

	echo "---------------------------------"
	response=`curl -s -H "Content-Type:application/x-www-form-urlencoded" --data "${post_param}" --data-urlencode "${post_data}" ${post_request_path}`
	echo -e "\nBaidu face detection response as follow:"
#	echo ${response}
	echo ${response} > ./response/response.txt
}

function face_response_parse()
{
	echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
	echo "人脸识别检测结果(测试程序，只显示重要信息)："
	echo ""

	result_num=`echo ${response} | tr ',' '\n' | grep "result_num" | cut -d ':' -f 2`
	echo ">>人脸数量：${result_num} 张"

#	grep -Po '(?<=left":)[0-9]+'  显示left后的正则表达式匹配数字
	location=`echo ${response} | grep -Po '(?<=location":){.*?}' | tr "{" "\t  " | tr "}," " " | sed 's/\"left\"/距左/g' | sed 's/\"top\"/距顶/g' | sed 's/\"width\"/宽度/g' | sed 's/\"height\"/高度/g'` 
	echo ">>位置(px)："
	echo "${location}"

	age=`echo ${response} | tr ',' '\n' | grep "age" | cut -d ':' -f 2 | cut -d '.' -f 1 | tr '\n' '    '`
	echo ">>年龄：${age}"

	gender=`echo ${response} | tr ',' '\n' | grep -w "gender" | cut -d ':' -f 2 | sed 's/\"//g' | tr '\n' '    ' | sed 's/female/女/g' | sed 's/male/男/g'`
	echo ">>性别：${gender}"

	race=`echo ${response} | tr ',' '\n' | grep -w "race" | cut -d ':' -f 2 | sed 's/\"//g' | tr '\n' '    '`
	echo ">>种族：${race}"

	beauty=`echo ${response} | tr ',' '\n' | grep "beauty" | cut -d ':' -f 2 | awk '{ printf "%d\n",$1}' | tr '\n' '    '`
	echo ">>美丑：${beauty}"

	expression=`echo ${response} | tr ',' '\n' | grep -w "expression" | cut -d ':' -f 2 | tr '\n' '    ' | sed 's/0/没笑/g' | sed 's/1/微笑/g' | sed 's/2/大笑/g'`
	echo ">>笑容：${expression}"

	glasses=`echo ${response} | tr ',' '\n' | grep -w "glasses" | cut -d ':' -f 2 | tr '\n' '    ' | sed 's/0/无眼镜/g' | sed 's/1/普通眼镜/g' | sed 's/2/墨镜/g'`
	echo ">>眼镜：${glasses}"
}

base64_image $1
get_access_token
send_post_request
face_response_parse
