# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-07 17:12:49
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-07 17:13:50

import tkinter

top = tkinter.Tk()

quit = tkinter.Button(top, text='hello world!', command=top.quit)
quit.pack()

tkinter.mainloop()