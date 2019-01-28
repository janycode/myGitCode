# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 14:45:42
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 14:50:22


num = ["harden","lampard",3,34,45,56,76,87,78,45,3,3,3,87686,98,76]
print(num.count(3))
print(num.index(3))
for i in range(num.count(3)):      #获取3出现的次数
    ele_index = num.index(3)        #获取首次3出现的坐标
    num[ele_index]="3a"            #修改3为3a
print(num)