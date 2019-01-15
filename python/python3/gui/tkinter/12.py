# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 10:24:10
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 10:30:47

from tkinter import *

# 按钮交互
root = Tk()
frame1 = Frame(root)
frame2 = Frame(root)

var = StringVar()
var.set('请重试！\n您的操作不被允许！')
textLabel = Label(frame1,
                  textvariable=var,
                  justify=LEFT)  #左对齐
textLabel.pack(side=LEFT)

#显示图片
photo = PhotoImage(file='warning.png')
imageLabel = Label(root, image=photo)
imageLabel.pack(side=RIGHT)

def callback():
    var.set('吹吧你，我才不信呢！')

theButton = Button(frame2, text='我是超级管理员', command=callback)
theButton.pack()

frame1.pack(padx=10, pady=10)
frame2.pack(padx=10, pady=10)

mainloop()