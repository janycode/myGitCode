#!/usr/bin/env python3

import ftplib
import os
import socket

# 需提前在本地创建好FTP本地服务器，下载测试OK
HOST = '10.129.61.14'
DIRN = '/'
FILE = 'python3-library-en.pdf'

def main():
    try:
        f = ftplib.FTP(HOST)    # 连接到 FTP 服务器
    except(socket.error, socket.gaierror) as e:
        print ('ERROR: cannot reach %s' % HOST)
        return
    print ('*** Connected to host %s' % HOST)

    try:
        f.login()               # 尝试用“anonymous”登录
    except ftplib.error_perm:
        print ('ERROR: cannot longin anonymously')
        f.quit()
        return
    print ('*** Logged in as anonymous')

    try:
        f.cwd(DIRN)             # 进入指定路径
    except ftplib.error_perm:
        print ('ERROR: cannot CD to %s' % DIRN)
        f.quit()
        return
    print ('*** Changed to %s folder' % DIRN)

    try:
        f.retrbinary('RETR %s' % FILE, open(FILE, 'wb').write) # 下载文件，每接收到一块二进制数据的时候都会调用这个回调函数
    except ftplib.error_perm:
        print ('ERROR: cannot read file %s' % FILE)
        os.unlink(FILE)
    else:
        print ('*** Downloaded %s to CWD' % FILE)
    f.quit()

if __name__ == '__main__':
    main()