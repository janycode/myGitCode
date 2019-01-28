#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-23 11:17:47
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-23 11:35:33

# 定义装饰器函数funcA
def funcA(func):
    def wrapper(*args, **kw):
        print ('call %s():' % func.__name__)
        return func(*args, **kw)

    print ('111 -> funcA')             # run funcA
    num = 10
    func(num)                          # run funcB
    print ('333 -> funcA num:', num)   # run funcA

    wrapper(num)                       # call funcB, run funcB
    return wrapper


@funcA       # 使用装饰器 @funcA：先调用运行一次funcA
def funcB(n):
    print ('222 -> funcB n:', n)