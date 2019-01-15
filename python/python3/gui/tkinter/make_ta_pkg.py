# -*- coding: utf-8 -*-
# Author: yuan.jiang
# Data: 2018.11.23
# Description: Make package for TMC version(TA)

import os
import time
import shutil
import zipfile
import sqlite3
from hashlib import md5
from imp import reload
from unrar import rarfile
from tkinter import *

reload(rarfile)

# hel="Hey"
# print ("{}!".format(hel))

# print ("----------- Start to pack TMC version -----------")
# print ("\neg:")
# print ("Pack ver: Claro_Central_America_SIMLOCK_HH41NH_MI_02.00_01_20180507")
# print ("PTS  num: 052")
# print ("CU   Ref: HH41NH-2BC6AO3\n")

'''
while True:
    version_dir_name = input (">>> input version: ")
    pts_num          = input (">>> input PTS num: ")
    cu_ref           = input (">>> input CU  Ref: ")
    next_flag        = input ("Yes for next[Y]; Back for reinput[N]:")
    if next_flag == "Y" or next_flag == "y":
        break

while True:
    version_dir_name = raw_input (">>> input version: ")
    pts_num          = raw_input (">>> input PTS num: ")
    cu_ref           = raw_input (">>> input CU  Ref: ")
    next_flag        = raw_input ("Yes for next[Y]; Back for reinput[N]:")
    if next_flag == "Y" or next_flag == "y":
        break
'''

# print ("\nVersion: {}".format(version_dir_name))
# print ("PTS   num: {}".format(pts_num))
# print ("CU    Ref: {}\n".format(cu_ref))

version_dir_name = ""
pts_num = ""
cu_ref = ""

copyFileCounts = 0
globalWorkdir = ""

def parse_version_param(name):
    name_list = name.split('_')
    param_list = list(reversed(name_list))
    print ("param_list: {}".format(param_list))
    return param_list


def find_project_path(param_list, remote_p):
    print ("\n>>> Find_project_path")
    #current_path = os.getcwd()
    project_path_name = param_list[3] + '_' + param_list[0]
    project_path = os.path.join(remote_p, project_path_name)  #10.129.61.36
    
    print ("remote_path: {}".format(remote_p))
    print ("project_path_name: {}".format(project_path_name))
    print ("project_path: {}".format(project_path))
    
    return project_path


def find_version_path(project_path, version_name):
    print ("\n>>> Find_version_path")
    version_path = os.path.join(project_path, version_name)

    print ("version_path: {}".format(version_path))
    return version_path


def create_ta_package_dir(project_path, param_list):
    print ("\n>>> Create_ta_package_dir")

    global globalWorkdir
    # mkdir tmp dir
    globalWorkdir = os.getcwd()
    print ('globalWorkdir: {}'.format(globalWorkdir))
    tmp_dir = "workdir"
    if os.path.exists(tmp_dir):
        os.chdir(tmp_dir)
    else:
        os.mkdir(tmp_dir)
        os.chdir(tmp_dir)

    # mkdir ta package dir
    ta_package=param_list[0]+'_'+param_list[1]+param_list[2]+'_'+param_list[3]+ \
                '_'+param_list[4]+'_'+param_list[5]+'_P'+param_list[6]+'_'+ \
                param_list[7]+'_'+param_list[8]
    ta_package_path = os.path.join(os.getcwd(), ta_package)
    # ta_package_path = os.getcwd()

    print ("ta_package: {}".format(ta_package))
    print ("ta_package_path: {}".format(ta_package_path))

    # if not os.path.exists(ta_package_path):
    #     os.mkdir(ta_package_path)
    # else:
    #     print ("ta_package_path is existed, please check.")

    return ta_package_path


def copy_files_to_local(pj_name, ver_path, ta_path):
    print ("\n>>> Copy_files_to_local")

    ver_name = list(os.path.split(ver_path))[1] + '.rar'
    ver_file = os.path.join(ver_path, ver_name)

    if pj_name == 'HH40' or pj_name == 'HH41':
        # .rar package, need exit.
        if os.path.isfile(ver_file):
            sys.exit(0)
        # .zip package.
        ver_name = list(os.path.split(ver_path))[1] + '.zip'
        ver_file = os.path.join(ver_path, ver_name)
        print ("HH40/HH41 ver_file: ", ver_file)
        if os.path.isfile(ver_file):
            os.mkdir(ta_path)
            cp_cmd = "xcopy " + ver_path + "\\* " + ta_path + " /s"          # copy all files
    else:
        cp_cmd = "copy " + ver_file + " " + os.getcwd()                         # copy one file

    print ("ver_name: ", ver_name)
    print ("ver_file: ", ver_file)
    print ("cp_cmd: ", cp_cmd)

    os.system(cp_cmd)


def unrar_operation(ta_path):
    print ("\n>>> Judge_rar_version_pkg")
    print ("doing unrar, please wait...")

    files = os.listdir(ta_path)
    for file in files:
        if file.endswith(".zip"):
            return 0
        elif file.endswith(".rar"):
            ver_dir = os.path.splitext(file)[0]
            ver_rar = os.path.join(ta_path, ver_dir + '.rar')
            #cmd = "start winrar x -y -ibck " + ver_rar + " " + ta_path
            #file_news = os.path.splitext(file)[0] + '.zip'
            #small_board = os.path.splitext(file)[0] + '_small_board' #_Smallboard
            #main_board  = os.path.splitext(file)[0] + '_main_board'  #_Mainboard
            print ("ver dir    name: {}".format(ver_dir))
            print ("ver rar    name: {}".format(ver_rar))
            print ("ta path    name: {}".format(ta_path))
            rf = rarfile.RarFile(ver_rar)
            rf.extractall(ta_path)

            #print ("cmd            : {}".format(cmd))
            #print ("smallboard name: {}".format(small_board))
            #print ("mainboard  name: {}".format(main_board))

            rar_file = os.path.join(ta_path, file)
            cmd = "del " + rar_file
            os.system(cmd)
            print ("cmd: {}".format(cmd))
            print ("unrar success, delete .rar success.")
        else:
            print ("No such files of .zip or .rar package!!!")
            exit

def copy_files(sourceDir, targetDir):
    global copyFileCounts
    print ("sourceDir: {}".format(sourceDir))
    print ("Copying... Please have a rest...")

    for f in os.listdir(sourceDir):
        sourceF = os.path.join(sourceDir, f)
        targetF = os.path.join(targetDir, f)

        if os.path.isfile(sourceF):
            #create dir
            if not os.path.exists(targetDir):
                os.makedirs(targetDir)
            copyFileCounts += 1

            #Cover
            if not os.path.exists(targetF) or (os.path.exists(targetF) and (os.path.getsize(targetF) != os.path.getsize(sourceF))):
                open(targetF, "wb").write(open(sourceF, "rb").read())
                print ("{} {} copy complete.".format(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())), targetF))
                print ("Copying... Please have a rest...")
            else:
                print ("{} {} file is existed.".format(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())), targetF))

        if os.path.isdir(sourceF):
            #continue FotaImage dir
            if "FotaImage" in sourceF:
                continue
            else:
                copy_files(sourceF, targetF)

    print (u"{} Do dir number {}, done {} files".format(time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), sourceDir, copyFileCounts))


def touch_pts_txt(path):
    print ("\n>>> Touch_pts_txt")
    pts_filename = "PTS" + pts_num + ".txt"
    pts_file = os.path.join(path, pts_filename)
    print ("pts file: {}".format(pts_file))

    with open(pts_file, 'w') as f:
        f.write(pts_num)
    f.close()

    return pts_filename

def addFileIntoZipfile(srcDir, fp):
    if os.path.isfile(srcDir): #file, do zip
        fp.write(srcDir)
    else:
        for subpath in os.listdir(srcDir):  #all files
            subpath = os.path.join(srcDir, subpath)
            # subpath = win32api.GetShortPathName(subpath)  #solve long path issue.
            if os.path.isfile(subpath):
                fp.write(subpath)
            elif os.path.isdir(subpath):
                fp.write(subpath)
                addFileIntoZipfile(subpath, fp)
 
def zipCompress(srcDir, desZipfile):
    with zipfile.ZipFile(desZipfile, mode='a') as fp:
        addFileIntoZipfile(srcDir, fp)

def add_simlock_config(version_path, package_name):
    print ("\n>>> Add_simlock_config") 
    db_file = version_path + r"\FotaImage\jrd_resource\resource\sqlite3\factory_info.db3"
    print ("db_file: {}".format(db_file))
    db = sqlite3.connect (db_file)

    for item in db.execute ("select * from sim_config where items='NetworkCode'"):
        simlock_plmn = item[1]
        print ("simlock_plmn: {}".format(simlock_plmn))
    
    if simlock_plmn == '00000':
        print ("No simlock...")
        return 0
    else:
        print ("Normal simlock!!! simlock PLMN={}".format(simlock_plmn))
        os.chdir(version_path)
        os.mkdir("Simlock")
        #os.chdir(os.path.join(os.getcwd(), package_name))
        simlock_file = "Simlock/simlock.cfg"

        with open(simlock_file, 'w') as f:
            f.write('#' + package_name + '\n')
            f.write('[CODE]\n')
            f.write('PN=' + simlock_plmn + '\n')
            f.write('PU=NULL\n' + 'PP=NULL\n' + 'PC=NULL\n' + 'PF=NULL\n\n')
            f.write('[ACTION]\n' + 'PN=2\n' + 'PU=0\n' + 'PP=0\n' + 'PC=0\n' + 'PF=0\n')
        
        if os.path.exists(simlock_file):
            print ("Simlock config file create success...")

def generate_file_md5(fpath):
    '''return md5 value'''
    m = md5()
    a_file = open(fpath, 'rb')  #need binary.
    m.update(a_file.read())
    a_file.close()
    return m.hexdigest()

def delete_other_files(dir_name):
    print ("\n>>> Delete_other_files")
    files = os.listdir(ta_package_path)
    for file in files:
        if os.path.isdir(file) and file == "Simlock":
            #os.rmdir(file) # If dir is not null, then it occurs OSError
            shutil.rmtree(file)
            print ("Delete [{}] success...".format(file))
        if file.endswith(".xlsx") or file.endswith(".txt") or file.endswith(".doc") or file.endswith(".MD5") or file.endswith('.xml'):
            os.remove(file)
            print ("Delete [{}] success...".format(file))

def write_pts_num_into_txt(filepath, ta_pkg_name):
    print ("\n>>> write_pts_num_into_txt")

    lines = []
    f = open(filepath,'r')
    for line in f:
        lines.append(line)
    f.close()
    print (lines)

    line1_text = "Package Version: " + ta_pkg_name + '\n'
    lines[0] = line1_text

    pts_num_text = "PTS Number: P" + pts_num + '\n'
    lines[10] = pts_num_text

    cu_ref_text = "Cu-Ref: " + cu_ref + '\n'
    lines[11] = cu_ref_text
    # lines.insert(0, line1_text + '\n')
    # lines.insert(12, pts_num)           #write pts num into line 11
    s = ''.join(lines)
    f = open(filepath, 'w+')
    f.write(s)
    f.close()
    del lines[:]


def check_files_and_do_zip(pj_name, ta_package_path):
    print ("\n>>> check_files_and_do_zip")

    if pj_name == 'HH40' or pj_name == 'HH41':
        (package_path, ta_package_name) = os.path.split(ta_package_path)
        ta_package_name_zip = ta_package_name + ".zip"
        package_path = ta_package_path
    else:
        (package_path, ta_package_name) = os.path.split(ta_package_path)
        ta_package_name_zip = ta_package_name + ".zip"

    print ("ta path: {}".format(package_path))
    print ("ta name: {}".format(ta_package_name))

    files = os.listdir(package_path)
    for file in files:
        if file.endswith(".rar"):
            unrar_operation(package_path)
            # ver_dir_tmp = os.path.join(ta_package_path, version_dir_name)
            # print ("ver_dir_tmp: {}".format(ver_dir_tmp))

            # os.chdir(ta_package_path)
            os.rename(version_dir_name, ta_package_name)
            ta_zip_file_path = os.path.join(package_path, ta_package_name_zip)
            print ("ta_zip_file_path: {}".format(ta_zip_file_path))

            files_path = os.path.join(package_path, ta_package_name)
            print ("files_path: {}".format(files_path))

            # pts file
            pts_filename = touch_pts_txt(files_path)

            # simlock file
            simlock_dir = "Simlock"
            if os.path.isdir(simlock_dir):
                zipCompress(simlock_dir, ta_zip_file_path)
                print ("zip [simlock]dir into TA package finished.")
            else:
                add_simlock_config(files_path, ta_package_name)         #sqlite3: read simlock config.

            # release notes.txt
            release_txt = "Release Notes.txt"
            release_txt_path = os.path.join(files_path, release_txt)
            write_pts_num_into_txt(release_txt_path, ta_package_name)

            # zip TA package
            print ("DEBUG:current path = {}".format(os.getcwd()))
            print ("Packing TA dir to zip, please wait...")
            os.chdir(package_path)
            zipCompress(ta_package_name, ta_zip_file_path)
            print ("Packing TA zip success...  Start to send swver >>>")
            del_cmd = "rd /S /Q " + ta_package_name
            os.system(del_cmd)

        elif file.endswith('.xlsx'):
            del_cmd = "del *.xlsx"      # delete .xlsx file.
            os.system(del_cmd)

        elif file.endswith('.zip'):
            zipfile = os.path.join(ta_package_path, file)
            print (".zip filepath: {}".format(zipfile))
            os.chdir(ta_package_path)
            #shutil.unpack_archive(zipfile) #unzip
            #os.remove(zipfile)
            
            os.rename(zipfile, ta_package_name_zip)
            ta_zip_file_path = os.path.join(ta_package_path, ta_package_name_zip)
            print ("ta_zip_file_path: {}".format(ta_zip_file_path))

            #config.xml
            config_xml = os.path.join(ta_package_path, "config.xml")
            if not os.path.isfile(config_xml):
                cfg_src_path = os.path.join(ta_package_path, r"FotaImage\config.xml")
                cfg_dst_path = ta_package_path
                shutil.copy(cfg_src_path, cfg_dst_path)

            pts_filename    = touch_pts_txt(ta_package_path)
            simlock_dir = "Simlock"
            release_doc = "Release Notes.doc"
            test_md5    = "testmd5.MD5"
            config_xml  = "config.xml"

            cfg_xml_file_md5 = generate_file_md5(config_xml).upper()
            print ("config.xml md5: {}".format(cfg_xml_file_md5))
            with open(test_md5, 'a+') as f:
                test_md5_content = f.read()
                if "config.xml" in test_md5_content:
                    print ("config.xml md5 valud has writen in test.MD5.")
                else:
                    f.write(cfg_xml_file_md5 + '\t' + config_xml)

            if os.path.isdir(simlock_dir):
                zipCompress(simlock_dir, ta_zip_file_path)
                print ("zip [simlock]dir into TA package finished.")
            else:
                print ("simlock_dir is not existed... Start to create >>")
                #sqlite3: read simlock config.
                add_simlock_config(ta_package_path, ta_package_name)
                if os.path.exists(simlock_dir):
                    zipCompress(simlock_dir, ta_zip_file_path)
                print ("zip [simlock]dir into TA package finished.")

            if os.path.isfile(release_doc) and os.path.isfile(test_md5) and os.path.isfile(pts_filename) and os.path.isfile(config_xml):
                zipCompress(release_doc,  ta_zip_file_path)
                zipCompress(test_md5,     ta_zip_file_path)
                zipCompress(pts_filename, ta_zip_file_path)
                zipCompress(config_xml,   ta_zip_file_path)
                print ("zip [pts_num,test_md5,release_doc,config_xml]files into TA package finished.")
                # delete_other_files(ta_package_path)
                print ("\n>>> All work done, good job! <<<\n")
                print ("The last step: \n\tYou need to manually modify the [Release Notes.doc] into the zip.")
            else:
                print ("One of release_doc/test_md5/pts_file is not existed...")
        else:
            print ("Warning: Not found other .rar or .zip files.")

    return ta_zip_file_path

def send_to_swver(pj_name, pro_path, zip_f):
    print ("\n>>> send_to_swver")
    print ("zip_f: {}".format(zip_f))

    # mkdir ta dir into swver.
    ta_dir = os.path.join(pro_path, os.path.split(zip_f)[1].split('.zip')[0])
    print ("ta_dir: {}".format(ta_dir))
    os.mkdir(ta_dir)

    send_cmd = "copy " + zip_f + " " + ta_dir
    os.system(send_cmd)

    open_cmd = "explorer " + ta_dir
    os.system(open_cmd)

    global globalWorkdir
    os.chdir(globalWorkdir)
    del_dir = globalWorkdir + '\\workdir'
    print ("globalWorkdir: ", globalWorkdir)
    if os.listdir(globalWorkdir):
        print ("workdir is not null, start to clean >>")
        del_cmd = "rd /S /Q " + del_dir
        os.system(del_cmd)
    else:
        print ("workdir is cleared.")

    # if pj_name == 'HH40' or pj_name == 'HH41':
    #     os.chdir(globalWorkdir)
    #     local_ta_dir = os.path.split(os.path.split(zip_f)[0])[1]
    #     pritn ("local_ta_dir: ", local_ta_dir)
    #     if os.path.isdir(local_ta_dir):
    #         del_cmd = "rd /S /Q " + local_ta_dir
    #         print ("del_cmd: ", del_cmd)
    #         os.system(del_cmd)
    # else:
    #     if os.path.exists(zip_f):
    #         del_cmd = "del " + zip_f
    #         print ("del_cmd: ", del_cmd)
    #         os.system(del_cmd)

    print ("All work done!!!")


if __name__ == '__main__':
# -------------------------------------------------------------- #
    window = Tk()
    window.title('Make TMC Tool V1.0 - by yuan.jiang (2018.12.28)')
    # window.geometry('600x200')

    default_ver = 'Rogers_Canada_HH41NH_MT_02.00_03_20180718'
    default_pts = '066'
    default_cuf = 'HH41NH-CU1A2B3'
    # -------------global values-------------- #
    # input_ver = ""
    # input_pts = ""
    # input_cuf = ""
    # -------------global values-------------- #

    Label(window, text='Version:').grid(row=0, column=0)
    Label(window, text='PTS num:').grid(row=1, column=0)
    Label(window, text='CU   Ref:').grid(row=2, column=0)
    e1 = Entry(window, width=66, fg='blue')
    e2 = Entry(window, width=66, fg='blue')
    e3 = Entry(window, width=66, fg='blue')
    e1.insert(END, default_ver)
    e2.insert(END, default_pts)
    e3.insert(END, default_cuf)
    e1.grid(row=0, column=1, padx=10, pady=5)
    e2.grid(row=1, column=1, padx=10, pady=5)
    e3.grid(row=2, column=1, padx=10, pady=5)

    Label(window, text='Description: ').grid(row=4, column=0)
    v1 = StringVar()
    e4 = Entry(window, width=66, textvariable=v1, stat="readonly")
    e4.grid(row=4, column=1, padx=10, pady=5)
    v1.set("此工具现仅用于HH40/HH41(非.rar)，MW40/MW41(无限制)。")


    #当输入内容时可以获取输入内容到对应变量
    def aciton():
        global version_dir_name
        global pts_num
        global cu_ref
        global v1
        # print('Version: %s' % e1.get())
        # print('PTS num: %s' % e2.get())
        # print('CU  Ref: %s' % e3.get())
        version_dir_name = e1.get()
        pts_num = e2.get()
        cu_ref = e3.get()
        print('Input Version: %s' % version_dir_name)
        print('Input PTS num: %s' % pts_num)
        print('Input CU  Ref: %s' % cu_ref)

# -------------------------------------------------------------- #
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

        # ------------------------------- swver values ------------------------------- #
        # if param_list[4] == 'MW41NF' or param_list[4] == 'MW41CL':
        if project_name == 'MW41':
            proj_name = "MW41GL"
        # elif param_list[4] == 'MW40V' or param_list[4] == 'MW40CJ' or param_list[4] == 'MW40VD':
        elif project_name == 'MW40':
            proj_name = "MW40_MIFI40"
        # elif param_list[4] == 'HH41V' or param_list[4] == 'HH41NH' or param_list[4] == 'HH41CM':
        elif project_name == 'HH41':
            proj_name = "HH41_HUB41"
        # elif param_list[4] == 'HH40V':
        elif project_name == 'HH40':
            proj_name = "HH40_HUB40"
        else:
            print ("ERROR: please help to check [version] which you input, it's error.")

        remote_path = "\\\\10.129.61.36\\swver\\" + proj_name + "\\"
        # ------------------------------- swver values ------------------------------- #

        project_path = find_project_path(param_list_new, remote_path)
        version_path = find_version_path(project_path, version_dir_name)
        ta_package_path = create_ta_package_dir(project_path, param_list_new)
        # copy files to local path.
        copy_files_to_local(project_name, version_path, ta_package_path)

        # package .zip
        ta_zip_file = check_files_and_do_zip(project_name, ta_package_path)  # local operating.

        # send to swver(10.129.61.36)
        send_to_swver(project_name, project_path, ta_zip_file)

        #end_flag = raw_input (">>> press [Enter] to exit...")
        end_flag = input (">>> press [Enter] to exit...")

        ta_name = os.path.split(ta_package_path)[1]
        v1.set("Make {} success!".fromat(ta_name))
# -------------------------------------------------------------- #
    def clear_text():
        e1.delete(0, END)
        e2.delete(0, END)
        e3.delete(0, END)

    Button(window, text='开始', width=10, command=aciton).grid(row=3, column=0, sticky=W, padx=20, pady=5)
    Button(window, text='清空', width=10, command=clear_text).grid(row=3, column=1, sticky=E, padx=20, pady=5)

    mainloop()

 