# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-07 17:19:43
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-08 10:39:13
import os
from time import sleep
from tkinter import *

# 一个目录文件浏览器
class DirList(object):
    def __init__(self, initdir=None):
        self.top = Tk()
        # 顶部版本信息
        self.label = Label(self.top, text='Directory Lister v1.1')
        self.label.pack()

        self.cwd = StringVar(self.top)      #保存当前所在目录名

        # 顶部挨着的路径显示
        self.dirl = Label (self.top, fg='blue', font=('Helvetica', 12, 'bold'))
        self.dirl.pack()

        # 显示文件列表的容器
        self.dirfm = Frame(self.top)        #纯容器
        self.dirsb = Scrollbar(self.dirfm)  #滚动
        self.dirsb.pack(side=RIGHT, fill=Y)
        self.dirs = Listbox(self.dirfm, height=15, width=50, yscrollcommand=self.dirsb.set)  #选项列表
        self.dirs.bind('<Double-l>', self.setDirAndGo)
        self.dirsb.config(command=self.dirs.yview)
        self.dirs.pack(side=LEFT, fill=BOTH)
        self.dirfm.pack()

        # 用于用户输入目录地址的输入框
        self.dirn = Entry(self.top, width=50, textvariable=self.cwd)
        self.dirn.bind('<Return>', self.doLS)
        self.dirn.pack()

        # 三个功能按钮Clear / List Directory / Quit
        self.bfm = Frame(self.top)          #容器
        self.clr = Button(self.bfm, text='Clear', command=self.clrDir, activeforeground='white', activebackground='blue')
        self.ls = Button(self.bfm, text='List Directory', command=self.doLS, activeforeground='white', activebackground='green')
        self.quit = Button(self.bfm, text='Quit', command=self.top.quit, activeforeground='white', activebackground='red')
        self.clr.pack(side=LEFT)
        self.ls.pack(side=LEFT)
        self.quit.pack(side=LEFT)
        self.bfm.pack()

        if initdir:
            self.cwd.set(os.curdir)
            self.doLS()

    # Clear按钮的回调
    def clrDir(self, ev=None):
        self.cwd.set('')

    # 设置要遍历的目录
    def setDirAndGo(self, ev=None):
        self.last = self.cwd.get()
        self.dirs.config(selectbackground='red')
        check = self.dirs.get(self.dirs.curselection())
        if not check:
            check = os.curdir
        self.cwd.set(check)
        self.doLS()

    # 安全检查、目录中信息替换
    def doLS(self, ev=None):
        error = ''
        tdir = self.cwd.get()
        if not tdir:
            tdir = os.curdir

        if not os.path.exists(tdir):
            error = tdir + ': no such file'
        elif not os.path.isdir(tdir):
            error = tdir + ': not a directory'

        if error:
            self.cwd.set(error)
            self.top.update()
            sleep(2)
            if not (hasattr(self, 'last') and self.last):
                self.last = os.curdir
            self.cwd.set(self.last)
            self.dirs.config(selectbackground='LightSkyBlue')
            self.top.update()
            return

        self.cwd.set('FETCHING DIRECTORY CONTENTS...')
        self.top.update()
        dirlist = os.listdir(tdir)
        dirlist.sort()
        os.chdir(tdir)

        self.dirl.config(text=os.getcwd())
        self.dirs.delete(0, END)
        self.dirs.insert(END, os.curdir)
        self.dirs.insert(END, os.pardir)
        for eachFile in dirlist:
            self.dirs.insert(END, eachFile)
        self.cwd.set(os.curdir)
        self.dirs.config(selectbackground='LightSkyBlue')


def main():
    d = DirList(os.curdir)
    mainloop()

if __name__ == '__main__':
    main()