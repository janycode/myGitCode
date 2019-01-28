# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 14:35:10
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 14:39:45

L = ['Hello', 'World', 'IBM', 'Apple']

for s in L:
    s = s.lower()
    print (s)

# 列表推导式
def lbtds():
    print ([s.lower() for s in L])

lbtds()