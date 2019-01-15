# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 11:15:52
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 11:16:57

from tkinter import *

# 消息和范围选择框
root = Tk()
root.title('Module')


#消息组件：Message
m1 = Message(root, text='这是一个消息：', width=100)
m1.pack()


m2 = Message(root, text='这是一\n则骇人听闻的长长长长长长长消息！', width=100)
m2.pack()


#输入组件：Spinbox  (可指定输入范围)
s1 = Spinbox(root, from_=0, to=5)
s1.pack()
s2 = Spinbox(root, values=('zero', 'one', 'two', 'three', 'four', 'five'))
s2.pack()


mainloop()