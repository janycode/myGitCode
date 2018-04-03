# Author	: yuan.jiang
# Date		: 2018.04.02
# Language  : python
# Filename	£ºCracker
# -*- coding: utf-8 -*-

#import Tkinter as tk	# 'Tkinter' for win7
import tkinter as tk	# 'tkinter' for win10
import os, re, time

main_width  = 300
main_height = 100

# Execute windows commands interface and return output.
def execCmd(cmd):
    r = os.popen(cmd)
    text = r.read()
    r.close()
    return text

class Break:
    def __init__(self, master):
        frame1 = tk.Frame(master) # Text layer
        frame1.pack(padx=5, pady=5)

        frame2 = tk.Frame(master) # Button layer
        frame2.pack(padx=5, pady=5)

        frame3 = tk.Frame(master) # Text layer
        frame3.pack(padx=5, pady=5)

        self.run_here = tk.Button(frame2, text='Start', fg='black', bd=2, width=20,  command=self.get_cmd_result)
        self.run_here.pack()

        self.var = tk.StringVar()
        self.var.set('>>> Break ByondCompare 4 <<<')
        textLabel = tk.Label(frame1,
                          textvariable=self.var,
                          justify=tk.LEFT)
        textLabel.pack(side=tk.LEFT)

        theLabel = tk.Label(frame3, text='Author: yuan.jiang TCL-2018', fg='gray')
        theLabel.pack()

    # Core function for breaking.
    def get_cmd_result(self):
        cmd = "whoami /user | find \"S-1-\""
        result = execCmd(cmd).split(" ")
        sid = result[1].replace("\n", "")
        print ("SID: " + sid)
        break_cmd = "reg delete \"HKEY_USERS\\" + sid + "\\Software\\Scooter Software\\Beyond Compare 4\" /v CacheId /f"
        print (break_cmd + "\n")
        execCmd(break_cmd)
        time.sleep(0.3)
        # Display success, only for success.
        self.var.set ("Success!!! Add 30 days to use.")

def center_window(root, width, height):  
    screenwidth = root.winfo_screenwidth()  
    screenheight = root.winfo_screenheight()  
    size = '%dx%d+%d+%d' % (width, height, (screenwidth - width)/2, (screenheight - height)/2)  
    #print(size)  
    root.geometry(size)


root = tk.Tk()
root.title('Cracker')
root.iconbitmap("..\\logo.ico")
center_window(root, main_width, main_height)
root.maxsize(main_width, main_height)
root.minsize(main_width, main_height)

app = Break(root)

root.mainloop()

# Package this python file to *.exe for windows running:
# <windows>> pip install pyinstaller
# <windows>> pyinstaller -F -w -i logo.ico sourcefile.py
# Dest path: .\dist\*.exe
# End.