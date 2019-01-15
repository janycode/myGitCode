# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-28 14:43:17
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-28 17:04:13


from tkinter import *
root = Tk()
root.title("Entry Test")
v1 = StringVar()
v2 = StringVar()
v3 = StringVar()
#设置entry为只读属性
Entry(root, width=30,textvariable=v1, stat="readonly").pack()
v1.set("readonly")
#默认情况下下Entry的状态为normal
Entry(root, width=30,textvariable=v2).pack()
v2.set("normal")
#将输入的内容用密文的形式显示
entry = Entry(root, width=30,textvariable=v3)
v3.set("password")
entry.pack()
entry["show"] = "*"
root.mainloop()