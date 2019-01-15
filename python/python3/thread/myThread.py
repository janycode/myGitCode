#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import threading
from time import ctime

# 为了让 MyThread 实现的 Thread 的子类更加通用
# 将这个子类移到一个专门的模块中
# 并添加了可调用的getResult()方法来取得返回值。
class MyThread(threading.Thread):
    def __init__(self, func, args, name=''):
        threading.Thread.__init__(self)  # 必须要先调用基类的构造
        self.name = name
        self.func = func
        self.args = args

    def getResult(self):
        return self.res

    def run(self):
        print ('starting', self.name, 'at:', ctime())
        self.res = self.func(*self.args)
        print (self.name, 'finished at:', ctime())