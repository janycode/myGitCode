#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-02-13 14:05:10
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-02-14 13:49:04

import urllib.request

''' urllib.request.urlopen(url) 获取网页源代码 '''
response = urllib.request.urlopen('https://www.python.org/')
# print (response.read().decode('utf-8'))
print (type(response))
# print (dir(response))
print (response.status)
print (response.getheaders())
print (response.getheader('Server'))
