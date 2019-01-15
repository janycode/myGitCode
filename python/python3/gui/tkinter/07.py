# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-28 14:00:47
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-28 14:05:57

import tkinter as tk

window = tk.Tk()
window.title('small tool')
window.geometry('600x300')

# scale尺度部件
l = tk.Label(window, bg='yellow', width=20, text='empty')
l.pack()

def print_selection(v):
    l.config(text='you have selected '+v)

s = tk.Scale(window, label='try me', from_=1, to=10, orient=tk.HORIZONTAL, length=400, showvalue=0, tickinterval=2, resolution=0.01, command=print_selection)
s.pack()


window.mainloop()