#-*- coding: utf-8 -*-

import tkinter as tk
# from tkinter import *

def gui_window():
    top = tk.Tk()   # 根窗口(顶层窗口)，只能有一个

    label = tk.Label(top, text='Hello, tkinter!') # Label文本/图像控件
    label.pack()    # pack管理和显示控件

    quit = tk.Button(top, text='QUIT', command=top.quit, bg='gray', fg='white') # Button 按钮控件
    quit.pack(fill=tk.X, expand=1)


if __name__ == "__main__":
    gui_window()


    tk.mainloop()   # 主循环（显示窗体）