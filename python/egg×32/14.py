# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 14:53:32
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 14:53:39


list1 = [2,3,8,4,9,5,6]
list2 = [5,6,10,17,11,2]
list3 = list1 + list2
print(list3)              #不去重只进行两个列表的组合
print(set(list3))         #去重，类型为set需要转换成list
print(list(set(list3)))