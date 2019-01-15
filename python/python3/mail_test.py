#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from smtplib import SMTP as smtp
import poplib
from email.parser import Parser

# s = smtp('smtp.263.net')
# tolist = ["yuan.jiang@cienet.com.cn", "yuqi.wu@cienet.com.cn"]
# msg = '''\
#       From: yuan.jiang@cienet.com.cn
#       Subject: testin'...

#       This is a test. '''

# s.sendmail("yuan.jiang@cienet.com.cn", tolist, msg)
# s.quit()

email = str(input('请输入账号:'))
password = str(input('请输入授权码：'))
if '@163.' in email:
    pop3_server = 'pop3.163.com'
elif '@126.' in email:
    pop3_server = 'pop3.126.com'  # 网易授权服务器地址
elif '@qq.com' in email:  # 腾讯
    pop3_server = 'pop.qq.com'
elif '@sina.com' or '@2008.sina' or '@51uc.com' in email:  # 新浪
    pop3_server = 'pop.sina.com'
elif '@188.com' in email:
    pop3_server = 'pop3.188.com'
else:
    logger.info('不支持此邮箱%s，或程序出错' % email)
    print('暂不支持此邮箱')
    exit()

# 连接到POP3服务器:
print(pop3_server)
server = poplib.POP3_SSL(pop3_server)
# 可以打开或关闭调试信息:
server.set_debuglevel(1)
# 可选:打印POP3服务器的欢迎文字:
print(server.getwelcome().decode('utf-8'))

# 身份认证:
server.user(email)
server.pass_(password)

# stat()返回邮件数量和占用空间:
# print('Messages: %s. Size: %s' % server.stat())
# list()返回所有邮件的编号:
resp, mails, octets = server.list()

# 获取最新一封邮件, 注意索引号从1开始:
index = len(mails)

for i in range(1, index + 1):
    resp, lines, octets = server.retr(i)
    # lines存储了邮件的原始文本的每一行,
    # 可以获得整个邮件的原始文本:
    try:
        msg_content = b'\r\n'.join(lines).decode('utf-8')
    except:
        continue
    # 稍后解析出邮件:
    msg = Parser().parsestr(msg_content)
    print(msg)
server.quit()