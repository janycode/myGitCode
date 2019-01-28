# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-16 18:02:08
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-16 18:04:19

def calc(*numbers):
    sum = 0
    for n in numbers:
        sum = sum + n * n
        print ('n*n=', n*n)
    return sum

print (calc(2, 4, 5))