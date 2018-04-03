# -*- coding: utf-8 -*-
from tkinter import *      # 导入 Tkinter 库
import time,datetime,winsound,threading
root = Tk() 
lbl = Label(root,text ="hello,world",width = 30,height = 2)
def tick():
  global timer
  timer = threading.Timer(1.0,tick,"")
  currenttime = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
  lbl.config(text = currenttime)  
  currenttime1 = datetime.datetime.now()
  if currenttime1.minute == 0 and currenttime1.second == 0 and currenttime1.hour > 5 and currenttime1.hour < 23:
    winsound.Beep(1000,5000)
    #其中600表示声音大小，1000表示发生时长，1000为1秒
  lbl.pack() # 将小部件放置到主窗口中
  timer.start()
if __name__ == "__main__":
  timer = threading.Timer(1.0,tick,"")
  timer.start()

root.mainloop()
