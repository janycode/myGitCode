#!/usr/bin/python
# coding=utf-8

import os
import time

sourceDir = r"./source"
targetDir = r"./target"
copyFileCounts = 0

def copyFiles(sourceDir, targetDir):
    global copyFileCounts
    print ("sourceDir: {}".format(sourceDir))
    for f in os.listdir(sourceDir):
        sourceF = os.path.join(sourceDir, f)
        targetF = os.path.join(targetDir, f)

        if os.path.isfile(sourceF):
            #create dir
            if not os.path.exists(targetDir):
                os.makedirs(targetDir)
            copyFileCounts += 1
            #print ("数量: {}".format(copyFileCounts))

            #不存在/存在覆盖
            if not os.path.exists(targetF) or (os.path.exists(targetF) and (os.path.getsize(targetF) != os.path.getsize(sourceF))):
                open(targetF, "wb").write(open(sourceF, "rb").read())
                print (u"{} {} copy complete.".format(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())), targetF))
            else:
                print (u"{} {} file is existed.".format(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())), targetF))

        if os.path.isdir(sourceF):
            copyFiles(sourceF, targetF)

    print (u"{} 当前处理文件夹{}已处理{}个文件".format(time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), sourceDir, copyFileCounts))

if __name__ == "__main__":
    try:
        import psyco
        psyco.profile()
    except ImportError:
        pass
    copyFiles(sourceDir,targetDir)

