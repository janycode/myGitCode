# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 14:17:50
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 14:29:38

# 方法1：
def fac1():
    num = int(input('请输入一个数字：'))
    factorial = 1
    # 查看数字是否是负数、0、正数
    if num < 0:
        print ('负数没有阶乘！')
    elif num == 0:
        print ('0的阶乘为1')
    else:
        for i in range(1, num+1):
            factorial = factorial*i
        print ('%d 的阶乘为 %d' % (num, factorial))

# 方法2：
def fac2():
    num = int(input('请输入一个数字：'))
    if num < 0:
        print ('负数没有阶乘！')
    elif num == 0:
        print ('0的阶乘为1')
    else:
        print ('%d 的阶乘为 %d' % (num, factorial(num)))
def factorial(n):
    result = n
    for i in range(1, n):
        result = result*i
    return result

if __name__ == '__main__':
    print ('方法1：')
    fac1()
    print ('方法2：')
    fac2()