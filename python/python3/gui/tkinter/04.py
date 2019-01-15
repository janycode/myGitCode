# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-28 13:37:21
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-28 13:54:19

import tkinter as tk

window = tk.Tk()
window.title('My UI Tool')
window.geometry('600x200')

# Entry & Text 输入, 文本框
def insert_point():
    var = e.get()
    t.insert('insert', var)

def insert_end():
    var = e.get()
    t.insert('end', var)

e = tk.Entry(window, show="*")
e.pack()

b1 = tk.Button(window, text='insert point', width=20, height=2, command=insert_point)
b1.pack()

b2 = tk.Button(window, text='insert end', command=insert_end)
b2.pack()

t = tk.Text(window, height=2)
t.pack()

window.mainloop()