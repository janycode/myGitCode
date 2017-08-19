#!/bin/bash
#for test add content from src_file to dest_file at specified place.

echo "hello, begin..."
echo ""

src_file=${PWD}"/src_file"
dest_file=${PWD}"/dest_file_dir/dest_file"

function for_test ()
{
	test=`sed -i '2i\insert this line' $dest_file`
	echo $test
	echo "****************"
	cat $dest_file
}

function add_content_src_to_dest_file ()
{
	delimit_line="==========================================="

	# sed -i "2i\\insert line" file 该sed命令使用的是-i参数指定i\选项，在第2行后插入内容
	# 2i\\ 拆解3部分：2为行号，i\为sed行下追加命令，\为转义字符(必须转义读取变量)
	# "" 双引号，保持引号内的字面值，可读\$转义后的变量内容，单引号不行。
	echo $delimit_line | sed -i "2i\\$delimit_line" $dest_file
	cat $src_file | while read line
	do
		echo $line | sed -i "3i\\$line" $dest_file
	done
	
	cat $dest_file
}


#for_test
add_content_src_to_dest_file

echo ""
echo "hey, end..."
exit 0
