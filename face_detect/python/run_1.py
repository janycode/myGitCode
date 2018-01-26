#!/usr/bin/python2
# -*- coding: UTF-8 -*-  

from aip import AipFace

# 定义常量
APP_ID="10701883"
API_KEY="8WRsAAo2AWGUtuT4GI1LZwbz"
SECRET_KEY="aIUxawGasEKlF3inR7PtedVnENYGGOTD"

# 初始化AipFace对象
aipFace = AipFace(APP_ID, API_KEY, SECRET_KEY)

# 读取图片
filePath = "image/0.jpg"
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

#print(type(result))
print(result)
