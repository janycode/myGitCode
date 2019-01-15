#!/usr/bin/env python3

from atexit import register
from random import randrange
from time import sleep, ctime
from threading import Thread, Lock, BoundedSemaphore

# 糖果机和信号量
lock = Lock()
MAX  = 5
candytray = BoundedSemaphore(MAX)

def refill():
    lock.acquire()
    print ('Refill candy...')
    try:
        candytray.release()
    except ValueError:
        print ('Full, skipping...')
    else:
        print ('OK')
    lock.release()

def buy():
    lock.acquire()
    print ('Buying candy...')
    if candytray.acquire(False):
        print ('OK')
    else:
        print ('empty, skipping...')
    lock.release()

def producer(loops):
    for i in range(loops):
        refill()
        sleep(randrange(3))

def consumer(loops):
    for i in range(loops):
        buy()
        sleep(randrange(3))

def _main():
    print(('starting at:', ctime()))
    nloops = randrange(2, 6)
    print(('THE candy machine (full with %d bars)!' % MAX))
    Thread (target=consumer, args=(randrange(nloops, nloops+MAX+2),)).start()
    Thread (target=producer, args=(nloops,)).start()

@register
def _atexit():
    print(('all DONE at:', ctime()))

if __name__ == '__main__':
    _main()