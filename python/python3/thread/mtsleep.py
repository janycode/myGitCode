#!/usr/bin/env python3

import threading
from time import sleep, ctime

loops = [5, 4, 3, 2, 1]

class ThreadFunc(object):
    def __init__(self, func, args, name=''):
        self.name = name
        self.func = func
        self.args = args
    def __call__(self):
        self.func(*self.args)

class MyThread(threading.Thread):
    def __init__(self, func, args, name=''):
        threading.Thread.__init__(self)  # 必须要先调用基类的构造函数
        self.name = name
        self.func = func
        self.args = args
    def getResult(self):
        return self.res
    def run(self):
        self.res = self.func(*self.args)

# callback函数
def loop(nloop, nsec):
    print ('>>start loop ', nloop, 'at: ', ctime())
    sleep(nsec)
    print ('  loop ', nloop, ' done at: ', ctime())

def main():
    print ('starting at: ', ctime())
    threads = []
    nloops = range(len(loops))

    for i in nloops:
        #t = threading.Thread(target=loop, args=(i, loops[i]))
        #t = threading.Thread(target=ThreadFunc(loop, (i, loops[i]), loop.__name__))
        t = MyThread(loop, (i, loops[i]), loop.__name__)  # 线程实例，三个都可以
        threads.append(t)

    for i in nloops:
        threads[i].start()    # 启动线程(多个)
    
    for i in nloops:
        threads[i].join()      # 线程结束回收资源
    
    print ('All done at: ', ctime())

if __name__ == '__main__':
    main()

