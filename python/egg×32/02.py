# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-16 17:59:26
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-16 18:01:51

def power(x, n):
    s = 1
    while n > 0:
        n = n - 1  # 控制循环次数
        s = s * x
        print ('s=', s)
    return s

if __name__ == '__main__':
    power(2, 32)