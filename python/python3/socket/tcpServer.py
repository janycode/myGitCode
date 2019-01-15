#!/usr/bin/env python3

# TCP时间戳服务器
from socket import *
from time import ctime
import os

HOST = ''
PORT = 21568
BUFSIZ = 1024
ADDR = (HOST, PORT)

tcpSerSock = socket(AF_INET, SOCK_STREAM)
tcpSerSock.bind(ADDR)
tcpSerSock.listen(5)

while True:
    print ("waiting for connection...")
    tcpCliSock, addr = tcpSerSock.accept()
    print (">>>connected from: {}".format(addr))

    while True:
        data = tcpCliSock.recv(BUFSIZ)
        if not data:
            break
        str_d = data.decode('utf-8')
        print ('recv: %s' % str_d)
        if str_d == 'ls':
            tcpCliSock.send(('%s' % (str(os.listdir()))).encode('utf-8'))
        else:
          tcpCliSock.send(('%s [%s] %s' % (os.name, ctime(), data)).encode('utf-8'))  # 此处需要对send的参数整体编码为utf-8

    tcpCliSock.close()
tcpSerSock.close()
