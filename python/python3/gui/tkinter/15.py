# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 10:59:51
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 11:05:59

from tkinter import *
import time
import hashlib  #用于获取文件的MD5值，检查内容是否有修改

# 文本组件之MD5
root = Tk()
root.title('link click')


text = Text(root, width=50, height=10)
text.pack()


text.insert(INSERT, 'I love www.baidu.com')
contents = text.get('1.0', END)


def getSig(contents):
    m = hashlib.md5(contents.encode())
    return m.digest()


sig = getSig(contents)


def check():
    for i in range(3):
        time.sleep(1)
        text.insert(INSERT, '\ninsert value %d into here.' % i)
    contents = text.get('1.0', END)
    if sig != getSig(contents):
        print('内容有修改，是否保存？')
    else:
        print('无任何修改！')


Button(root, text='检查', command=check).pack()


mainloop()