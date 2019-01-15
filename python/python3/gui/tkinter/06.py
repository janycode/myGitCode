# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-28 13:51:35
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-28 13:59:57

import tkinter as tk

window = tk.Tk()
window.title('small tool')
window.geometry('600x300')

# Radiobutton 选择按钮
var = tk.StringVar()
l = tk.Label(window, bg='yellow', width=20, text='empty')
l.pack()

def print_selection():
    l.config(text='you have selected ' + var.get())

r1 = tk.Radiobutton(window,
                    text='Option A',
                    variable=var,
                    value='A',
                    command=print_selection)
r1.pack()
r2 = tk.Radiobutton(window,
                    text='Option B',
                    variable=var,
                    value='B',
                    command=print_selection)
r2.pack()
r3 = tk.Radiobutton(window,
                    text='Option C',
                    variable=var,
                    value='C',
                    command=print_selection)
r3.pack()




window.mainloop()