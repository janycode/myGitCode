# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 14:58:25
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 15:01:12

import random
i = 1
a = random.randint(0,100)
b = int( input('请输入0-100中的一个数字\n然后查看是否与电脑一样：'))
while a != b:
    if a > b:
        print('你第%d次输入的数字小于电脑随机数字'%i)
        b = int(input('请再次输入数字:'))
    else:
        print('你第%d次输入的数字大于电脑随机数字'%i)
        b = int(input('请再次输入数字:'))
    i+=1
else:
    print('恭喜你，你第%d次输入的数字与电脑的随机数字%d一样'%(i,b))