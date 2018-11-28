#!/usr/bin/python

import os
import zipfile

def zip_ya(startdir,file_news):

    z = zipfile.ZipFile(file_news,'a',zipfile.ZIP_DEFLATED)
    for dirpath, dirnames, filenames in os.walk(startdir):
        fpath = dirpath.replace(startdir,'')
        fpath = fpath and fpath + os.sep or ''
        for filename in filenames:
            z.write(os.path.join(dirpath, filename),fpath+filename)
            print ('success')
    z.close()

if __name__=="__main__":
    dir1 = "./11111_Mainboard"
    dir2 = "./22222_Smallboard"
    file_news = "test" + '.zip'
    zip_ya(dir1, file_news)
    zip_ya(dir2, file_news)
