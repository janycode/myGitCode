# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-28 13:43:12
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-28 13:56:16

import tkinter as tk

window = tk.Tk()
window.title('My Small Tool')
window.geometry('600x300')

# Listbox 列表部件
var1 = tk.StringVar()
l = tk.Label(window, bg='yellow', width=4, textvariable=var1)
l.pack()

var2 = tk.StringVar()
var2.set((11, 22, 33, 44))

# 创建Listbox
lb = tk.Listbox(window, listvariable=var2)

list_items=[1, 2, 3, 4]
for item in list_items:
    lb.insert('end', item)
lb.insert(1, 'first')
lb.insert(2, 'second')
lb.delete(2)
lb.pack()


def print_selection():
    value = lb.get(lb.curselection())  # 获取当前选中的文本
    var1.set(value)

b1 = tk.Button(window, text='print selection', width=20, height=2, command=print_selection)
b1.pack()



window.mainloop()