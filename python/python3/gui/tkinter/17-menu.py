# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 11:08:08
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 11:26:11

from tkinter import *

# menu菜单栏
root = Tk()
root.title('Main Menu Show')


def callback():
    print('你好~')

def about():
    top = Toplevel()
    top.title('关于')
    top.geometry('300x50')

    msg = Message(top, width=200, text='Make TMC Tool V1.0\nyuan.jiang\n- 2018.12.28 -')
    msg.pack()


menubar = Menu(root)


#注册菜单：文件（下拉菜单）
filemenu = Menu(menubar, tearoff=False)  #来自主菜单，tearoff参数可让菜单窗口分离
filemenu.add_command(label='新建', command=callback)
filemenu.add_command(label='打开...', command=callback)
filemenu.add_separator()  #分割线
filemenu.add_command(label='保存', command=callback)
filemenu.add_separator()  #分割线
filemenu.add_command(label='退出', command=root.quit)
menubar.add_cascade(label='文件(W)', menu=filemenu)


#主菜单：编辑（下拉菜单）
editmenu = Menu(menubar, tearoff=False)  #来自主菜单
editmenu.add_command(label='撤销', command=callback)
editmenu.add_command(label='重做', command=callback)
editmenu.add_separator()  #分割线
editmenu.add_command(label='剪切', command=callback)
editmenu.add_command(label='复制', command=callback)
editmenu.add_command(label='粘贴', command=callback)
editmenu.add_separator()  #分割线
editmenu.add_command(label='全选', command=callback)
editmenu.add_separator()  #分割线
editmenu.add_command(label='查找...', command=callback)
menubar.add_cascade(label='编辑(B)', menu=editmenu)


#主菜单：多选√ checkbutton（下拉菜单）
openVar = IntVar()
saveVar = IntVar()
quitVar = IntVar()
optionmenu = Menu(menubar, tearoff=False)
optionmenu.add_checkbutton(label='多选项1', command=callback, variable=openVar)
optionmenu.add_checkbutton(label='多选项2', command=callback, variable=saveVar)
optionmenu.add_checkbutton(label='多选项3', command=callback, variable=quitVar)
menubar.add_cascade(label='选项(C)', menu=optionmenu)


#主菜单：单选√ radiobutton（下拉菜单）
otherVar = IntVar()
othermenu = Menu(menubar, tearoff=False)
othermenu.add_radiobutton(label='单选项1', command=callback, variable=otherVar, value=1)
othermenu.add_radiobutton(label='单选项2', command=callback, variable=otherVar, value=2)
othermenu.add_radiobutton(label='单选项3', command=callback, variable=otherVar, value=3)
menubar.add_cascade(label='其他(C)', menu=othermenu)


#内部菜单：按钮菜单 Menubutton
mb = Menubutton(root, text='按钮菜单...', relief=RAISED)
mb.pack()
openVar = IntVar()
saveVar = IntVar()
quitVar = IntVar()
optionmenu = Menu(mb, tearoff=False)
optionmenu.add_checkbutton(label='test', command=callback, variable=openVar)
optionmenu.add_checkbutton(label='test', command=callback, variable=saveVar)
optionmenu.add_checkbutton(label='test', command=callback, variable=quitVar)
mb.config(menu=optionmenu)


#内部菜单：选项菜单 OptionMenu
variable = StringVar()
variable.set('one')  #默认显示one
w = OptionMenu(root, variable, 'one', 'two', 'three')
w.pack()


#将列表添加到选项菜单 OptionMenu
OPTIONS = [
    '表项1',
    '对比2',
    '选项3',
    '其他4',
    '退出5'
    ]
var = StringVar()
var.set(OPTIONS[0])
o = OptionMenu(root, var, *OPTIONS)  #*星号解包可变参数列表为逐个元素
o.pack()


#主菜单：帮助
helpmenu = Menu(menubar, tearoff=False)
helpmenu.add_separator()  #分割线
helpmenu.add_command(label='关于...', command=about)
helpmenu.add_separator()  #分割线
menubar.add_cascade(label='帮助(F1)', menu=helpmenu)


#弹出菜单(暂用编辑菜单作为右键)
frame = Frame(root, width=512, height=200)
frame.pack()
def popup(event):
    editmenu.post(event.x_root, event.y_root)
frame.bind('<Button-3>', popup)  #Button-3为鼠标右键，1为左键，2为中键


root.config(menu=menubar)  #menu参数会将菜单设置添加到root根窗口


mainloop()