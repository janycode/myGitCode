# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 11:17:48
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 11:25:17

from tkinter import *


root = Tk()
root.title('Toplevel')


def create():
    top = Toplevel()
    #top.attributes('-alpha', 0.5) 设置弹出的顶级窗口透明度：50%
    top.title('Toplevel demo...')

    msg = Message(top, text='I love python...')
    msg.pack()


Button(root, text='创建顶级窗口', command=create).pack()  #点击出现顶级窗口


mainloop()