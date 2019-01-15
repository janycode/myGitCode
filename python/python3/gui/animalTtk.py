# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-08 10:57:37
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-08 11:03:29
from tkinter import Tk, Spinbox
from tkinter.ttk import Style, Label, Button, Combobox

top = Tk()
Style().configure("TButton", foreground='white', background='red')

Label(top, text='Animals(in pairs; min:pair, max: dozen)').pack()
Label(top, text='Number:').pack()

Spinbox(top, from_=2, to=12, increment=2, font='Helvetica -14 bold').pack()

Label(top, text='Type:').pack()

Combobox(top, values=('dog', 'cat', 'hamster', 'python')).pack()

Button(top, text='QUIT', command=top.quit, style="TButton").pack()

top.mainloop()