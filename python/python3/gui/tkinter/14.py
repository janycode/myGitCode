# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 10:47:12
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 10:59:46

from tkinter import *
import webbrowser as wb

# 鼠标滑过链接，点击链接
root = Tk()
root.title('GUI link show')


text = Text(root, width=30, height=5)
text.pack()


text.insert(INSERT, 'I love baidu.com!')
text.tag_add('link', '1.7', '1.16')
text.tag_config('link', foreground='blue', underline=True)


def show_arrow_cursor(event):
    text.config(cursor='arrow')


def show_xterm_cursor(event):
    text.config(cursor='xterm')


def click(event):
    wb.open('http://www.baidu.com')


#绑定事件
text.tag_bind('link', '<Enter>', show_arrow_cursor)  #<Enter>鼠标进入
text.tag_bind('link', '<Leave>', show_xterm_cursor)  #<Enter>鼠标离开
text.tag_bind('link', '<Button-1>', click)  #<Enter>鼠标点击


mainloop()