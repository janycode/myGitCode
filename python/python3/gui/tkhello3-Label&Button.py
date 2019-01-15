# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-07 17:14:08
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-07 17:15:28
import tkinter

top = tkinter.Tk()

hello = tkinter.Label(top, text='hello, world!')
hello.pack()

quit = tkinter.Button(top, text='QUIT', command=top.quit, bg='red', fg='white')
quit.pack(fill=tkinter.X, expand=1)

tkinter.mainloop()