# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-28 10:45:27
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-28 13:54:07

import tkinter as tk

window = tk.Tk()
window.title('Make TMC Tool V1.0')
window.geometry("600x200")

# Label & Button 标签和按钮
l = tk.Label(window,
             text='OMG!, this is TK!',
             bg='gray',
             font=('SimHei', 12),
             width=20,
             height=2)
l.pack()

var = tk.StringVar()  # 文字变量存储器
l = tk.Label(window,
             textvariable=var,  # textvariable可变文本
             bg='green',
             font=('SimHei', 12),
             width=20,
             height=2)
l.pack()

on_hit = False
def hit_me ():
    global on_hit
    if on_hit == False:
        on_hit = True
        var.set('You hit me!!!')
    else:
        on_hit = False
        var.set("")

b = tk.Button(window,
              text='hit me!',
              width=20,
              height=2,
              command=hit_me)
b.pack()



window.mainloop()
