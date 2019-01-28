# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 14:43:15
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 14:43:55

import os

def print_dir(filepath):
  for i in os.listdir(filepath):
    path = os.path.join(filepath, i)
    if os.path.isdir(path):
      print_dir(path)
    if path.endswith(".html"):
      print(path)

filepath = "D:"
print_dir(filepath)