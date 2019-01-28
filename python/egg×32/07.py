# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 14:35:57
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 14:42:01

import os

def print_dir(filepath):
    if filepath == '':
        print ('请给一个非空路径！')
    else:
        for i in os.listdir(filepath):
            print (os.path.join(filepath, i))

filepath = input('请输入一个路径：')
print_dir(filepath)