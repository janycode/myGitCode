# -*- coding: utf-8 -*-
# Author: yuan.jiang
# Data: 2018.11.23
# Description: Make package for TMC version(TA)
# Notice: 实际运行或打包exe文件时需删除包含中文字符所有行，包括注释行。

import os
import time
import shutil
import zipfile
import sqlite3
from hashlib import md5

hel="Hey"
print ("{}!".format(hel))

print ("-----------Start to pack TMC version-----------")
print ("\neg:")
print ("Pack ver: Claro_Central_America_SIMLOCK_HH41NH_MI_02.00_01_20180507")
print ("PTS  num: 052")
print ("CU   Ref: HH41NH-2BC6AO3\n")

'''
while True:
    version_dir_name = input (">>> input version: ")
    pts_num          = input (">>> input PTS num: ")
    cu_ref           = input (">>> input CU  Ref: ")
    next_flag        = input ("Yes for next[Y]; Back for reinput[N]:")
    if next_flag == "Y" or next_flag == "y":
        break
'''
# input()用于python3，在打包exe的时候用的python2智能使用raw_input()
while True:
    version_dir_name = raw_input (">>> input version: ")
    pts_num          = raw_input (">>> input PTS num: ")
    cu_ref           = raw_input (">>> input CU  Ref: ")
    next_flag        = raw_input ("Yes for next[Y]; Back for reinput[N]:")
    # 考虑使用习惯，兼容大小写
    if next_flag == "Y" or next_flag == "y":
        break


print ("\nVersion: {}".format(version_dir_name))
print ("PTS   num: {}".format(pts_num))
print ("CU    Ref: {}\n".format(cu_ref))

copyFileCounts = 0

def parse_version_param(name):
    name_list = name.split('_')                 #以_分隔字符串
    param_list = list(reversed(name_list))      #反序列表中的元素
    print ("param_list: {}".format(param_list))
    return param_list


def find_project_path(param_list):
    print ("\n>>> Find_project_path")
    current_path = os.getcwd()                                      #获取当前工作路径
    project_path_name = param_list[3] + '_' + param_list[0]
    project_path = os.path.join(current_path, project_path_name)    #组合文件名到路径末尾生成完整的文件路径
    
    print ("current_path: {}".format(current_path))
    print ("project_path_name: {}".format(project_path_name))
    print ("project_path: {}".format(project_path))
    
    return project_path


def find_version_path(project_path, version_name):
    print ("\n>>> Find_version_path")
    version_path = os.path.join(project_path, version_name)     #组合文件名到路径末尾生成完整的文件路径

    print ("version_path: {}".format(version_path))
    return version_path


def create_ta_package_dir(project_path, param_list):
    print ("\n>>> Create_ta_package_dir")
    ta_package=param_list[0]+'_'+param_list[1]+param_list[2]+'_'+param_list[3]+ \
                '_'+param_list[4]+'_'+param_list[5]+'_P'+param_list[6]+'_'+ \
                param_list[7]+'_'+param_list[8]
    ta_package_path = os.path.join(project_path, ta_package)    #组合文件名到路径末尾生成完整的文件路径

    print ("ta_package: {}".format(ta_package))
    print ("ta_package_path: {}".format(ta_package_path))

    os.chdir(project_path)                                      #改变当前工作目录

    if os.path.exists(ta_package):                              #判断文件/文件夹是否存在
        print ("TA package is existed...needn't to create.")
    else:
        os.mkdir(ta_package_path)
        print ("TA package is created successfully.")

    return ta_package_path

#未调用
def zip_ya(startdir,file_news):

    z = zipfile.ZipFile(file_news,'a',zipfile.ZIP_DEFLATED)
    for dirpath, dirnames, filenames in os.walk(startdir):
        fpath = dirpath.replace(startdir,'')
        fpath = fpath and fpath + os.sep or ''
        for filename in filenames:
            z.write(os.path.join(dirpath, filename),fpath+filename)
            print ('zip [{}] success'.format(filename))
    z.close()

#未调用
def judge_rar_version_pkg(ver_path):
    print ("\n>>> Judge_rar_version_pkg")

    files = os.listdir(ver_path)
    for file in files:
        if file.endswith(".zip"):
            return 0
        elif file.endswith(".rar"):
            ver_rar = os.path.join(ver_path, os.path.splitext(file)[0] + '.rar')
            cmd = "start winrar x -y -ibck " + ver_rar + " " + ver_path
            file_news = os.path.splitext(file)[0] + '.zip'
            small_board = os.path.splitext(file)[0] + '_small_board' #_Smallboard
            main_board  = os.path.splitext(file)[0] + '_main_board'  #_Mainboard

            print ("cmd            : {}".format(cmd))
            print ("ver rar    name: {}".format(ver_rar))
            print ("zip file   name: {}".format(file_news))
            print ("smallboard name: {}".format(small_board))
            print ("mainboard  name: {}".format(main_board))
            print ("unrar the .rar package file, need long time...")

            os.system(cmd)
            zip_ya(small_board, file_news)
            zip_ya(main_board, file_news)
            os.remove(file)
        else:
            print ("No such files of .zip & .rar package!!!")
            exit


def copy_files(sourceDir, targetDir):
    global copyFileCounts                           #全局统计拷贝文件数量
    print ("sourceDir: {}".format(sourceDir))
    for f in os.listdir(sourceDir):                 #遍历路径(参数)下所有文件
        sourceF = os.path.join(sourceDir, f)
        targetF = os.path.join(targetDir, f)

        if os.path.isfile(sourceF):                 #判断是否存在该文件，路径中如果有中文需要转换编码格式eg：os.path.isfile(u'D:\\文件.txt')
            #create dir
            if not os.path.exists(targetDir):
                os.makedirs(targetDir)
            copyFileCounts += 1

            #Cover
            if not os.path.exists(targetF) or (os.path.exists(targetF) and (os.path.getsize(targetF) != os.path.getsize(sourceF))):
                open(targetF, "wb").write(open(sourceF, "rb").read())       #从源文件中以二进制读取的内容作为写入目标文件的缓冲信息实现单文件拷贝
                print ("{} {} copy complete.".format(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())), targetF))  #log中时间打印
                print ("Copying... Please have a rest...")
            else:
                print ("{} {} file is existed.".format(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())), targetF))

        if os.path.isdir(sourceF):
            #continue FotaImage dir
            if "FotaImage" in sourceF:              #判断不需要拷贝的目录，进行跳过
                continue
            else:
                copy_files(sourceF, targetF)        #递归遍历目录下所有文件

    print (u"{} Do dir number {}, done {} files".format(time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), sourceDir, copyFileCounts))


def touch_pts_txt(ta_package_path):
    print ("\n>>> Touch_pts_txt")    
    pts_filename = "PTS" + pts_num + ".txt"                 #来自程序接收的输入信息，全局变量
    pts_file = os.path.join(ta_package_path, pts_filename)  #组文件的完整路径
    print ("pts file: {}".format(pts_file))

    with open(pts_file, 'w') as f:
        f.write(pts_num)
    f.close()

    return pts_filename

#zip压缩接口Begin: zipCompress()
def addFileIntoZipfile(srcDir, fp):
    if os.path.isfile(srcDir):              #file, do zip
        fp.write(srcDir)
    else:
        for subpath in os.listdir(srcDir):  #all files
            subpath = os.path.join(srcDir, subpath)
            if os.path.isfile(subpath):
                fp.write(subpath)
            elif os.path.isdir(subpath):
                fp.write(subpath)
                addFileIntoZipfile(subpath, fp)
 
def zipCompress(srcDir, desZipfile):
    with zipfile.ZipFile(desZipfile, mode='a') as fp:
        addFileIntoZipfile(srcDir, fp)
#zip压缩接口End: zipCompress()

def add_simlock_config(version_path, package_name):
    print ("\n>>> Add_simlock_config") 
    db_file = version_path + r"\FotaImage\jrd_resource\resource\sqlite3\factory_info.db3"       #原始路径加r，防止识别转移字符
    print ("db_file: {}".format(db_file))
    db = sqlite3.connect (db_file)                                                              #连接sqlite3数据库文件

    for item in db.execute ("select * from sim_config where items='NetworkCode'"):              #执行sqlite3数据库命令，返回一个结果列表
        simlock_plmn = item[1]
        print ("simlock_plmn: {}".format(simlock_plmn))
    
    if simlock_plmn == '00000':
        print ("No simlock...")
        return 0
    else:
        print ("Normal simlock!!! simlock PLMN={}".format(simlock_plmn))
        print ("DEBUG:current path = {}".format(os.getcwd()))
        os.mkdir("Simlock")                                                                     #创建目录
        #os.chdir(os.path.join(os.getcwd(), package_name))
        simlock_file = "Simlock/simlock.cfg"

        with open(simlock_file, 'w') as f:                                                      #创建可写入文件
            f.write('#' + package_name + '\n')
            f.write('[CODE]\n')
            f.write('PN=' + simlock_plmn + '\n')
            f.write('PU=NULL\n' + 'PP=NULL\n' + 'PC=NULL\n' + 'PF=NULL\n\n')
            f.write('[ACTION]\n' + 'PN=2\n' + 'PU=0\n' + 'PP=0\n' + 'PC=0\n' + 'PF=0\n')
        
        if os.path.exists(simlock_file):                                                        #判断写入文件是否存在
            print ("Simlock config file create success...")

#md5生成接口Begin：generate_file_md5()
def generate_file_md5(fpath):
    '''return md5 value'''
    m = md5()
    a_file = open(fpath, 'rb')  #need binary.
    m.update(a_file.read())
    a_file.close()
    return m.hexdigest()
#md5生成接口End：generate_file_md5()

def delete_other_files(dir_name):
    print ("\n>>> Delete_other_files")
    files = os.listdir(ta_package_path)
    for file in files:
        #如果是目录则删除目录下所有文件: shutil.rmtree(dir)
        if os.path.isdir(file) and file == "Simlock":
            #os.rmdir(file) # If dir is not null, then it occurs OSError
            shutil.rmtree(file)
            print ("Delete [{}] success...".format(file))
        #如果是指定文件后缀则进行对应删除: file.endswith(".xxx")
        if file.endswith(".xlsx") or file.endswith(".txt") or file.endswith(".doc") or file.endswith(".MD5") or file.endswith('.xml'):
            os.remove(file)
            print ("Delete [{}] success...".format(file))
    

def copy_files_and_append_zip(version_path, ta_package_path):
    print ("\n>>> Copy_files_and_unzip")

    #judge_rar_version_pkg(version_path)
    
    copy_files(version_path, ta_package_path)

    #config.xml
    cfg_src_path = os.path.join(version_path, r"FotaImage\config.xml")      #将文件组到完整路径中
    cfg_dst_path = ta_package_path
    print ("config.xml src path: {}".format(cfg_src_path))
    print ("config.xml dst path: {}".format(ta_package_path))
    shutil.copy(cfg_src_path, cfg_dst_path)                                 #拷贝单个文件shutil.copy(sourceF, destF)

    (package_path, ta_package_name) = os.path.split(ta_package_path)        #拆分文件名与路径，与os.path.join()刚好相反
    print ("ta path1: {}".format(package_path))
    print ("ta name: {}".format(ta_package_name))
    ta_package_name = ta_package_name + ".zip"

    files = os.listdir(ta_package_path)
    for file in files:
        if file.endswith(".zip"):
            print (".zip filename: {}".format(file))
            zipfile = os.path.join(ta_package_path, file)
            print (".zip filepath: {}".format(zipfile))
            os.chdir(ta_package_path)
            #shutil.unpack_archive(zipfile) #unzip
            #os.remove(zipfile)
            
            os.rename(file, ta_package_name)                                #修改文件名之前最好修改当前工作目录
            ta_zip_file_path = os.path.join(ta_package_path, ta_package_name)
            print ("ta_zip_file_path: {}".format(ta_zip_file_path))

            print ("ta path2: {}".format(ta_package_path))
            #simlock_dir = os.path.join(ta_package_path, "Simlock")
            #release_doc = os.path.join(ta_package_path, "Release Notes.doc")
            #test_md5    = os.path.join(ta_package_path, "testmd5.MD5")
            pts_filename    = touch_pts_txt(ta_package_path)
            simlock_dir = "Simlock"
            release_doc = "Release Notes.doc"
            test_md5    = "testmd5.MD5"
            config_xml  = "config.xml"

            cfg_xml_file_md5 = generate_file_md5(config_xml).upper()
            print ("config.xml md5: {}".format(cfg_xml_file_md5))
            with open(test_md5, 'a+') as f:                                 #a+模式，即可读也可以追加内容
                test_md5_content = f.read()
                if "config.xml" in test_md5_content:
                    print ("config.xml md5 value has writen in test.MD5.")
                else:
                    f.write(cfg_xml_file_md5 + '\t' + config_xml)

            if os.path.isdir(simlock_dir):
                zipCompress(simlock_dir, ta_zip_file_path)
                print ("zip [simlock]dir into TA package finished.")
            else:
                print ("simlock_dir is not existed...")
                #sqlite3: read simlock config.
                add_simlock_config(version_path, ta_package_name)
                if os.path.exists(simlock_dir):
                    zipCompress(simlock_dir, ta_zip_file_path)
                print ("zip [simlock]dir into TA package finished.")

            if os.path.isfile(release_doc) and os.path.isfile(test_md5) and os.path.isfile(pts_filename) and os.path.isfile(config_xml):
                zipCompress(release_doc,  ta_zip_file_path)
                zipCompress(test_md5,     ta_zip_file_path)
                zipCompress(pts_filename, ta_zip_file_path)
                zipCompress(config_xml,   ta_zip_file_path)
                print ("zip [pts_num,test_md5,release_doc,config_xml]files into TA package finished.")
                delete_other_files(ta_package_path)
                print ("\n>>> All work done, good job! <<<\n")
                print ("The last step: \n\tYou need to manually modify the [Release Notes.doc] into the zip.")
            else:
                print ("One of release_doc/test_md5/pts_file is not existed...")


if __name__ == '__main__':
    param_list = parse_version_param(version_dir_name)
    date_str = param_list[0]
    version_num = param_list[1]
    fixed_version = param_list[2]
    customID = param_list[3]
    project_name = param_list[4][0:4]
    variant_name = param_list[4][4:6]
    custom_name = '_'.join(reversed(param_list[5:]))
    
    param_list_new=[custom_name, project_name, variant_name, customID, fixed_version, version_num, pts_num, date_str, cu_ref]

    for each in param_list_new:
        print (each)
    print ("\n")
    
    project_path = find_project_path(param_list_new)
    version_path = find_version_path(project_path, version_dir_name)
    ta_package_path = create_ta_package_dir(project_path, param_list_new)
   
    copy_files_and_append_zip(version_path, ta_package_path)

    end_flag = raw_input (">>> press [Enter] to exit...")                   #For package .exe file by raw_input() function.
    #end_flag = input (">>> press [Enter] to exit...")