#!/usr/bin/env python
# -*- coding: utf-8 -*-
import serial
import os
from time import sleep


if __name__ == '__main__':
	serial = serial.Serial('/dev/sg3', 9600, timeout = 3600)
	if serial.isOpen():
		print("open success")
	else:
		print("open failed")
	while True:
		send_data = input("input a data: ")
		send_data = send_data + '\r\n'
		serial.write(send_data.encode())
		data=serial.read(1)#阻塞读直到读出第一个数据，然后用serial.inWaiting()计算出接收缓冲区还有多少个数据，使用read读出来
		sleep(0.1) #有些AT指令的回复太长，延迟一段时间，希望开发板的串口已经将AT指令的回复已经全部接收到缓冲区
		data = (data + serial.read(serial.inWaiting())).decode()
		print(data)
