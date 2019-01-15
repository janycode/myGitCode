# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 13:20:15
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 13:20:40

from tkinter import *
from tkinter import messagebox    #messagebox()需要单独导入
from tkinter import filedialog    #filedialog()需要单独导入
from tkinter import colorchooser  #colorchooser()需要单独导入
from tkinter.messagebox import *  #用户使用showinfo()


#警告对话框
showinfo(title='test', message='警告')


#消息对话框
result = messagebox.askokcancel('demo', '发射核弹？')  #返回值是True或False
print(result)  #根据用户按下了确定还是取消做进一步的操作


#文件对话框
root = Tk()
def callback1():
    filename = filedialog.askopenfilename(defaultextension='.py')  #指定文件后缀
    print(filename)  #返回的是文件的完整路径


Button(root, text='打开文件', command=callback1).pack()


#颜色选择对话框
def callback2():
    color_data = colorchooser.askcolor()  #调用windows的颜色选择器
    print(color_data)  #选择红色打印：((255.99609375, 0.0, 0.0), '#ff0000')


Button(root, text='选择颜色', command=callback2).pack()


mainloop()