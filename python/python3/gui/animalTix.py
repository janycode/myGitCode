# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-08 10:46:47
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-08 10:52:35
# 成对的动物要移走，而动物的总数在 2～12 个之间。 Control 控件用于跟踪动物总数，而 ComboBox 控件则包含了可以选择的动物种类。默认情况下动物的数量为 2 个，且没有动物种类被选中。

from tkinter import Label, Button, END
from tkinter.tix import Tk, Control, ComboBox

top = Tk()
top.tk.eval('package require Tix')

lb = Label (top, text='Animals(in pairs; min:pair, max: dozen')
lb.pack()

ct = Control(top, label='Number:', integer=True, max=12, min=12, value=2, step=2)
ct.label.config(font='Helvetica -14 bold')
ct.pack()

cb = ComboBox(top, label='Type:', editable=True)
for animal in ('dog', 'cat', 'hamster', 'python'):
    cb.insert(END, animal)
cb.pack()

qb = Button(top, text='QUIT', command=top.quit, bg='red', fg='white')
qb.pack()

top.mainloop()