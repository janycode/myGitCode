#!/bin/bash
# Web path: https://console.bce.baidu.com/ai/
# Test type: 人脸识别 >> 人脸检测 & 人脸对比
# Application: face_detect & face_match
# AppID: 10701883
# Author: jiangyuan

current_path=${PWD}
#echo ${current_path}

# Baidu Key Info #
face_detect_token_path="https://aip.baidubce.com/oauth/2.0/token"
face_detect_api_key="8WRsAAo2AWGUtuT4GI1LZwbz"
face_detect_secret_key="aIUxawGasEKlF3inR7PtedVnENYGGOTD"

# Baidu POST path #
face_detect_post_path="https://aip.baidubce.com/rest/2.0/face/v2/detect"
face_match_post_path="https://aip.baidubce.com/rest/2.0/face/v2/match"

# Internal paramter #
max_face_num=50
face_fields=age,beauty,expression,faceshape,gender,glasses,landmark,race,qualities

#curl -i -k 'https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=${face_detect_api_key}&client_secret=${face_detect_secret_key}'

function base64_image() 
{
	img_1_filename=`echo $1 | awk -F '/' '{print $2}' | awk -F '.' '{print $1}'`
	base64_file1=${current_path}"/encode/base64_"${img_1_filename}".txt"
	base64 $1 > ${base64_file1}

	if [ -n "$2" ]; then
		img_2_filename=`echo $2 | awk -F '/' '{print $2}' | awk -F '.' '{print $1}'`
		base64_file2=${current_path}"/encode/base64_"${img_2_filename}".txt"
		base64 $2 > ${base64_file2}
		base64_file=${current_path}"/encode/base64_file.txt"
		urlencode_file=${current_path}"/encode/urlencode_file.txt"
#		echo -e "images=\c" > ${base64_file}
		# 釜底抽薪法读取文件 #
		while read LINE
		do 
			echo $LINE >> ${base64_file}
		done < ${base64_file1}
		echo -e ",\c" >> ${base64_file}
		# 釜底抽薪法读取文件 #
		while read LINE
		do
			echo $LINE >> ${base64_file}
		done < ${base64_file2}
#		echo -e `cat ${base64_file1}`"\c" >> ${base64_file}
#		echo -e ",\c" >> ${base64_file}
#		echo -e `cat ${base64_file2}`"\c" >> ${base64_file}
			
#		base64_str1=`cat ${base64_file1}`
#		base64_str2=`cat ${base64_file2}`
#		base64_str="images="${base64_str1}","${base64_str2}
#		echo ${base64_str} > ${base64_file}
	else
		echo "The 2nd param is empty, only one image."
	fi

}

function get_access_token() 
{
#	echo "---------------------------------"
	cmd="curl -s -k ${face_detect_token_path}?grant_type=client_credentials&client_id=${face_detect_api_key}&client_secret=${face_detect_secret_key}"
#	echo "cmd: ${cmd}"
	access_token_response=`$cmd`
#	echo "access_token_response: ${access_token_response}"
	access_token=`echo ${access_token_response} | awk -F ':' '{print $2}' | cut -d ',' -f 1 | sed 's/\"//g'` 
	echo "access_token: ${access_token}"
	echo "Get access_token success..."
}

function send_post_request() 
{
	#Face detect
	if [ -z "$2" ]; then
		post_request_path=${face_detect_post_path}"?access_token="${access_token}
		base64_data=`cat ${base64_file1}`
		post_param="max_face_num="${max_face_num}"&face_fields="${face_fields}
		post_data="image="${base64_data}
		echo -e "-------------------------------\nBaidu face detect response as follow:"
		response=`curl -s -H "Content-Type:application/x-www-form-urlencoded" --data "${post_param}" --data-urlencode "${post_data}" ${post_request_path}`
	#Face match
	elif [ -n "$2" ]; then
		post_request_path=${face_match_post_path}"?access_token="${access_token}
		echo -e "-------------------------------\nBaidu face match response as follow:"
		response=`curl -s -H "Content-Type:application/x-www-form-urlencoded" --data-urlencode "images" --data "=" --data-urlencode @"${base64_file}" ${post_request_path}`
#		response=`curl -s -X POST -k -H "Content-Type:application/x-www-form-urlencoded" --data "images=" --data-urlencode @${urlencode_file} -i ${post_request_path}`
	else
		echo "The param error...please check."
	fi
	echo "post_request_path: ${post_request_path}"

	echo ${response}
#echo ${response} > ./response/response.txt
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

#Start here.
if [ $# -eq 1 ]; then
	echo "param cnt: $#"
	base64_image $1 $2
	get_access_token
	send_post_request $1 $2
	face_response_parse
elif [ $# -eq 2 ]; then
	echo "param cnt: $#"
	base64_image $1 $2
	get_access_token
	send_post_request $1 $2
#face_response_parse
else
	echo "param cnt: $#"
fi















