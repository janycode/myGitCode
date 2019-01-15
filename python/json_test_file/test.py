
s_path = r"J:\myGitCode\python\json_test_file\json_test_file"
d_path = r"J:\myGitCode\python\json_test_file\json_tmp"
string = "\"req\""
result = []

def find_dest_str_from_file(path, string):
    f = open(path)
    for line in f:
        if string in line:
            (key, value) = line.split(':', 1) #以:分隔出来的第1个元素
            (dest, tmp) = value.split(',', 1) #以,分隔出来的第1个元素
            #print (dest.strip().strip('\"'))
            result.append(dest.strip().strip('\"')) #先去除两端空格，再去除两端双引号，追加到列表
    f.close()

def write_result_to_file(path):
    f = open(path, 'w')
    print ("\n".join(result), file=f) #将通过\n连接列表中的元素，并写入文件
    f.close()

find_dest_str_from_file(s_path, string)
write_result_to_file(d_path)
