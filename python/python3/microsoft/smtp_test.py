#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-28 13:10:38
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-28 13:14:35
import smtplib

smtp = smtplib.SMTP()
smtp.connect("smtp.yeah.net", "25")
smtp.login('usr', 'pwd')
smtp.sendmail('yuan.jiang@jancom.com', 'yuqi.wu@jancom.com',
              'From: yuan.jiang@jancom.com\r\nTo: yuqi.wu@jancom.com\r\nSubject: This is a email form python demo.\r\n\r\nJust have a test!~~')
smtp.quit()