#!/usr/bin/env python3

# TCP时间戳客户端
from socket import *

HOST = 'localhost'  # HOST = '127.0.0.1'
PORT = 21568
BUFSIZ = 1024
ADDR = (HOST, PORT)

tcpCliSock = socket(AF_INET, SOCK_STREAM)
tcpCliSock.connect(ADDR)

while True:
	data = input('> ').encode("utf-8")  # 编码需要用utf-8
	if not data:
		break
	tcpCliSock.send(data)
	data = tcpCliSock.recv(BUFSIZ).decode()
	if not data:
		break
	print(data)

tcpCliSock.close()