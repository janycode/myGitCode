# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 14:44:25
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 14:44:35


dict1 = {"A":"a","B":"b","C":"c"}
dict2 = {y:x for x,y in dict1.items()}
print(dict2)