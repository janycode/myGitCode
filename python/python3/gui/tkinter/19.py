# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2018-12-29 11:17:03
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2018-12-29 11:17:12

from tkinter import *


root = Tk()
root.title('Module')


#二窗格
'''
p = PanedWindow(orient=VERTICAL)
p.pack(fill=BOTH, expand=1)


top = Label(p, text='top pane')
p.add(top)


bottom = Label(p, text='bottom pane')
p.add(bottom)
'''
#三窗格，同时显示隐藏布局线（showhandle=True, sashrelief=SUNKEN）
p = PanedWindow(showhandle=True, sashrelief=SUNKEN)
p.pack(fill=BOTH, expand=1)


left = Label(p, text='left pane')
p.add(left)


q = PanedWindow(orient=VERTICAL, showhandle=True, sashrelief=SUNKEN)
p.add(q)


top = Label(q, text='top pane')
q.add(top)
bottom = Label(q, text='bottom pane')
q.add(bottom)


mainloop()