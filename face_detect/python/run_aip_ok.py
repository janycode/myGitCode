#!/usr/bin/python2
# -*- coding: UTF-8 -*-  
# Author: jiangyuan
# Cteate: 2018.01.25
# Comments: Use baidu face detection interface library named "AipFace".
from aip import AipFace
import json


filePath = "image/0.jpg"

# 定义常量
APP_ID="10701883"
API_KEY="8WRsAAo2AWGUtuT4GI1LZwbz"
SECRET_KEY="aIUxawGasEKlF3inR7PtedVnENYGGOTD"

# 初始化AipFace对象
aipFace = AipFace(APP_ID, API_KEY, SECRET_KEY)

# 读取图片
def get_file_content(filePath):
	with open(filePath, 'rb') as fp:
		return fp.read()

# 定义参数变量
options = {
	'max_face_num': 50,
	'face_fields': "age,beauty,expression,faceshape,gender,glasses,race,qualities"
}

# 调用人脸属性检测接口
result = aipFace.detect(get_file_content(filePath), options)
# print(type(result))
print(result)

def parse_json_result(result_data):
	print ("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
	print ("Python face detection result:")
	print ("result_num = " + json.dumps(result_data['result_num']))
	print ("location =" +
		   " left:" + json.dumps(result_data['result'][0]['location']['left']) +
		   " top:" + json.dumps(result_data['result'][0]['location']['top']) +
		   " width:" + json.dumps(result_data['result'][0]['location']['width']) +
		   " height:" + json.dumps(result_data['result'][0]['location']['height']))
	print ("face_probability = " + json.dumps(result_data['result'][0]['face_probability']))
	print ("age = " + str(int(float(json.dumps(result_data['result'][0]['age'])))))
	print ("beauty = " + str(int(float(json.dumps(result_data['result'][0]['beauty'])))))
	print ("expression = " + str(int(float(json.dumps(result_data['result'][0]['expression'])))))
	print ("gender = " + json.dumps(result_data['result'][0]['gender']).strip("\""))
	print ("glasses = " + json.dumps(result_data['result'][0]['glasses']))
	print ("race = " + json.dumps(result_data['result'][0]['race']).strip("\""))

parse_json_result(result)
