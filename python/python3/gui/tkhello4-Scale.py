# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-07 17:15:44
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-07 17:18:41
import tkinter

from tkinter import *

def resize(ev=None):
    label.config(font='Helvetica -%d bold' % scale.get())

top = Tk()
top.geometry('250x150')

label = Label(top, text='hello world!', font='Helvetica -12 bold')
label.pack(fill=Y, expand=1)

scale = Scale(top, from_=10, to=40, orient=HORIZONTAL, command=resize)
scale.set(12)
scale.pack(fill=X, expand=1)

quit = Button(top, text='QUIT', command=top.quit, activeforeground='white', activebackground='red')
quit.pack()

mainloop()