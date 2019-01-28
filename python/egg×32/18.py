# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 15:02:30
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 15:05:45

def is_number(s):
    try:
        float(s)
        return  True
    except ValueError:
        pass
    try:
        import unicodedata
        unicodedata.numeric(s)
        return  True
    except(TypeError,ValueError):
        pass
    return False

t="a12d3"
print(is_number(t))


#方法二
t = "q123"
print(t.isdigit())   #检测字符串是否只由数字组成

#方法三
t = "123"
print(t.isnumeric())   #检测字符串是否只由数字组成,这种方法是只针对unicode对象
