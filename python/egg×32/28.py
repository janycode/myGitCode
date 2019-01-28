# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 15:24:05
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 15:24:06

# 写文件
with open("test.txt", "wt") as out_file:
    out_file.write("该文本会写入到文件中\n看到我了吧！")

# Read a file
with open("test.txt", "rt") as in_file:
    text = in_file.read()

print(text)