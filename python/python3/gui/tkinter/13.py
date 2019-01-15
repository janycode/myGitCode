# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 10:32:34
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 10:47:02

from tkinter import *

# 计算器
root = Tk()
root.title('计算器程序')

frame = Frame(root)
frame.pack(padx=10, pady=10)

v1 = StringVar()
v2 = StringVar()
v3 = StringVar()

def test(content):
    return content.isdigit()

testCMD = frame.register(test)
#focusout指定在当前输入框失去焦点时，代表输入完，会去调用test校验<tab>键可测试
e1 = Entry(frame, width=10, textvariable=v1, validate='key', \
           validatecommand=(testCMD, '%P')).grid(row=0, column=0)  #width的单位是字符数
Label(frame, text='+').grid(row=0, column=1)

e2 = Entry(frame, width=10, textvariable=v2, validate='key', \
           validatecommand=(testCMD, '%P')).grid(row=0, column=2)
Label(frame, text='=').grid(row=0, column=3)


e3 = Entry(frame, width=10, textvariable=v3, state='readonly').grid(row=0, column=4)

def calc():
    result = int(v1.get()) + int(v2.get())
    v3.set(str(result))

Button(frame, text='计算结果', command=calc).grid(row=1, column=2, pady=5)


mainloop()