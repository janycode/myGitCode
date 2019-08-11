# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-05 17:34:29
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-28 11:28:48
from tkinter import Tk
from tkinter.messagebox import showwarning
import win32com.client as win32

# 收件人列表
receiverList = ['yuan.jiang@jancom.com']

# 邮件签名
signContent = [
    '\r\n\nBest Regards!',
    '\r\n--------------------------------------',
    '\r\n姜源 Jiang Yuan',
    '\r\nConnected SW',
    '\r\nOffice: (0755) 3661 2553',
    '\r\nMobile: 18566206696',
    '\r\n--------------------------------------',
]

warn = lambda app:showwarning(app, 'Unknown Error, Outlook Exit!')
# RANGE = list(range(3, 8))

def outlook():
    app = 'Outlook'
    olook = win32.gencache.EnsureDispatch('%s.Application' % app)

    mail = olook.CreateItem(win32.constants.olMailItem)

    # 加入收件人
    # recip = mail.Recipients.Add('yuqi.wu@jancom.com')
    # recip = mail.Recipients.Add('yuan.jiang@jancom.com')
    for i in range(len(receiverList)):
        mail.Recipients.Add(receiverList[i])

    # 邮件Title
    subj = mail.Subject = 'Test Mail From Python-%s' % app

    # 邮件Content
    # body = ["Line %d" % i for i in RANGE]  #替换body
    body = ["测试批量发送、测试中文邮件内容。"]
    # body.insert(0, '%s\r\n' % subj)
    body.insert(0, 'Hi, all:\r\n')
    for i in range(len(signContent)):
        body.append(signContent[i])
    body.append("\r\nTh-th-th-that's all folks!")
    mail.Body = '\r\n'.join(body)

    # 发送邮件
    mail.Send()

    ns =  olook.GetNamespace("MAPI")
    obox = ns.GetDefaultFolder(win32.constants.olFolderOutbox)
    obox.Display()
    obox.Items.Item(1).Display()

    warn(app)
    olook.Quit()

if __name__ == '__main__':
    Tk().withdraw()
    outlook()