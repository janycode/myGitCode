# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-28 10:31:04
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-28 10:44:27

import tkinter as tk

def resize(ev=None):
    label.config(font='Helvetica -%d bold' % scale.get())

top = tk.Tk()
top.geometry('300x200')

label = tk.Label(top, text='Hello, world!', font='Helvetica -12 bold')
label.pack(fill=tk.Y, expand=1)

scale = tk.Scale(top, from_=10, to=40, orient=tk.HORIZONTAL, command=resize)
scale.set(12)
scale.pack(fill=tk.X, expand=1)

quit = tk.Button(top, text='QUIT', command=top.quit, activeforeground='white', activebackground='red')
quit.pack()

tk.mainloop()
