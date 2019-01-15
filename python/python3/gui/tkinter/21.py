# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 13:16:30
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 13:17:26

from tkinter import *


root = Tk()
root.title('place')


#place位置布局测试
'''
photo = PhotoImage(file='warning.png')
Label(root, image=photo).pack()  #按钮就会出现在图片的组件上，实现组件叠加显示


def callback():
    print('正中靶心！！！')


#relx,rely相对父组件root的位置，0.5正中间，1最右边，0最左边，anchor=CENTER居中显示
Button(root, text='射击', command=callback).place(relx=0.5, rely=0.5, anchor=CENTER)
'''
Label(root, bg='red').place(relx=0.5, rely=0.5, relheight=0.75, relwidth=0.75, anchor=CENTER)
Label(root, bg='yellow').place(relx=0.5, rely=0.5, relheight=0.5, relwidth=0.5, anchor=CENTER)
Label(root, bg='blue').place(relx=0.5, rely=0.5, relheight=0.25, relwidth=0.25, anchor=CENTER)


mainloop()