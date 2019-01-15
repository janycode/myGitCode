# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-28 14:11:39
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-28 14:17:26

import tkinter as tk

window = tk.Tk()
window.title('small tool')
window.geometry('600x300')

# Canvas部件 - 画布
canvas = tk.Canvas(window, bg='blue', height=100, width=200)
canvas.pack()

image_file = tk.PhotoImage(file='ins.gif')
image = canvas.create_image(10, 10, anchor='nw', image=image_file)

x0, y0, x1, y1= 50, 50, 80, 80
line = canvas.create_line(x0, y0, x1, y1)

oval = canvas.create_oval(x0, y0, x1, y1, fill='red')  #创建一个圆，填充色为`red`红色
arc = canvas.create_arc(x0+30, y0+30, x1+30, y1+30, start=0, extent=180)  #创建一个扇形
rect = canvas.create_rectangle(100, 30, 100+20, 30+20)   #创建一个矩形

def moveit():
    canvas.move(image, 0, 2)  # rect / arc / oval / line / image

b1 = tk.Button(window, text='move', width=20, height=2, command=moveit)
b1.pack()

window.mainloop()