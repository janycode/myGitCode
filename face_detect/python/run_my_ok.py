#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Author Name: jiangyuan
# Cteate Time: 2018.01.25
import base64
import urllib, urllib2, sys
import ssl
''' json or simplejson is OK '''
#import json
import simplejson as json

'''
Face detection
'''

image_file="image/0.jpg"

API_KEY="8WRsAAo2AWGUtuT4GI1LZwbz"				#AK
SECRET_KEY="aIUxawGasEKlF3inR7PtedVnENYGGOTD"	#SK
request_url="https://aip.baidubce.com/rest/2.0/face/v1/detect"

def get_access_token():
	host = 'https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=' + API_KEY + '&client_secret=' + SECRET_KEY
	#Get baidu http server response, strip access_token.
	token_request = urllib2.Request(host)
	token_request.add_header('Content-Type', 'application/json; charset=UTF-8')
	token_response = urllib2.urlopen(token_request)
	content = token_response.read()
	if content:
		print (">> access_token_response = " + content)
	if "access_token" in content:
		(key, value) = content.split(",", 1)
		(access_token_str, access_token) = key.split(":", 1)
		access_token = access_token.strip("\"")
		print (">> access_token = " + access_token)

	return access_token

def read_img_encode():
	#Open image by binary, convert image to base64 code.
	f = open(image_file, 'rb')
	img = base64.b64encode(f.read())
	#POST request params need urlencode.
	params = {"face_fields":"age,beauty,expression,faceshape,gender,glasses,race,qualities","image":img,"max_face_num":50}
	params = urllib.urlencode(params)

	return params

def get_http_response(access_token, request_data):
	#global request_url
	request_http_url = request_url + "?access_token=" + access_token
	print (">> request_url = " + request_url)
	print (">> request_http_url = " + request_http_url)
	print (">> access_token = " + access_token + "\n")
	request = urllib2.Request(url=request_http_url, data=request_data)
	request.add_header('Content-Type', 'application/x-www-form-urlencoded')
	response = urllib2.urlopen(request)
	content = response.read()
	if content:
		print (">> baidu response = " + content)
	result_data = json.loads(content)
	#Test date type.
	print (type(result_data))
	print (type(json.dumps(result_data['result_num'])))

	return result_data

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


if __name__ == "__main__":
	body = read_img_encode()
	access_token = get_access_token()
	result = get_http_response(access_token, body)
	parse_json_result(result)

