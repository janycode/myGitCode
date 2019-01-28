# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 14:44:52
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 14:45:00


for i in range(1,10):
    for j in range(1,i+1):
        print('%d x %d = %d \t'%(j,i,i*j),end='')   #通过指定end参数的值，可以取消在末尾输出回车符，实现不换行。
    print()
