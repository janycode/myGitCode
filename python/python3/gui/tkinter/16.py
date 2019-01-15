# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 11:06:19
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 11:06:26

from tkinter import *


root = Tk()
root.title('link click')


text = Text(root, width=50, height=10)
text.pack()


text.insert(INSERT, 'I love www.baidu.com')


def getIndex(text, index):
    return tuple(map(int, str.split(text.index(index), '.')))


start = '1.0'  #开头的位置，第1行的第0个下标位置
while True:
    pos = text.search('o', start, stopindex=END)  #查找文本中字符o的位置
    if not pos:
        break
    print('找到啦，位置是:', getIndex(text, pos))
    start = pos + '+1c'  #'+1c'指向下一个字符


mainloop()