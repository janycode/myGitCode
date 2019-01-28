# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 14:42:17
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 14:42:39

import os

def show_dir(filepath):
  for i in os.listdir(filepath):
    path = (os.path.join(filepath, i))
    print(path)
    if os.path.isdir(path):      #isdir()判断是否是目录
      show_dir(path)             #如果是目录，使用递归方法

filepath = "C:\Program Files\Internet Explorer"
show_dir(filepath)