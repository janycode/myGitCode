# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-10 15:25:14
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-10 15:29:35

from tkinter import Tk
from time import sleep
from tkinter.messagebox import showwarning
import win32com.client as win32

warn = lambda app: showwarning(app, 'Exit?')
RANGE = list(range(3, 8))

def word():
    app = 'Word'
    word = win32.gencache.EnsureDispatch('%s.Application' % app)
    doc = word.Documents.Add()
    word.Visible = True
    sleep(1)

    rng = doc.Range(0, 0)
    rng.InsertAfter('Python-to-%s Test\r\n\r\n' % app)
    sleep(1)

    for i in RANGE:
        rng.InsertAfter('Line %d\r\n' % i)
        sleep(i)
    rng.InsertAfter("\r\nTh-th-th-that's all folks!\r\n")

    warn(app)
    doc.Close(False)
    word.Application.Quit()

if __name__ == '__main__':
    Tk().withdraw()
    word()