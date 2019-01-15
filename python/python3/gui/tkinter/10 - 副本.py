# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-28 14:17:38
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-28 17:48:42
import time
from tkinter import *

window = Tk()
window.title('Make TMC Tool V1.0')
# window.geometry('600x200')

default_ver = 'Rogers_Canada_HH41NH_MT_02.00_03_20180718'
default_pts = '066'
default_cuf = 'HH41NH-CU1A2B3'
# -------------global values-------------- #
input_ver = ""
input_pts = ""
input_cuf = ""
# -------------global values-------------- #

Label(window, text='Version:').grid(row=0, column=0)
Label(window, text='PTS num:').grid(row=1, column=0)
Label(window, text='CU   Ref:').grid(row=2, column=0)
e1 = Entry(window, width=66, fg='gray')
e2 = Entry(window, width=66, fg='gray')
e3 = Entry(window, width=66, fg='gray')
e1.insert(END, default_ver)
e2.insert(END, default_pts)
e3.insert(END, default_cuf)
e1.grid(row=0, column=1, padx=10, pady=5)
e2.grid(row=1, column=1, padx=10, pady=5)
e3.grid(row=2, column=1, padx=10, pady=5)


Label(window, text='Output: ').grid(row=4, column=0)
v1 = StringVar()
e4 = Entry(window, width=66, textvariable=v1, stat="readonly")
e4.grid(row=4, column=1, padx=10, pady=5)
# v1.set("null")


#当输入内容时可以获取输入内容到对应变量
def aciton():
    global input_ver
    global input_pts
    global input_cuf
    global v1
    # print('Version: %s' % e1.get())
    # print('PTS num: %s' % e2.get())
    # print('CU  Ref: %s' % e3.get())
    input_ver = e1.get()
    input_pts = e2.get()
    input_cuf = e3.get()
    print('Input Version: %s' % input_ver)
    print('Input PTS num: %s' % input_pts)
    print('Input CU  Ref: %s' % input_cuf)
    v1.set("Start to make...")


def clear_text():
    e1.delete(0, END)
    e2.delete(0, END)
    e3.delete(0, END)

Button(window, text='开始', width=10, command=aciton).grid(row=3, column=0, sticky=W, padx=20, pady=5)
Button(window, text='清空', width=10, command=clear_text).grid(row=3, column=1, sticky=E, padx=20, pady=5)

mainloop()
