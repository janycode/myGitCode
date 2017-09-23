#!/bin/bash
# file function: Add version number to release history, git commit it, make package, copy package to swver.

#set -v on

Small_board="small"
Main_board="main"

function help ()
{
	echo -e "\nUsage: $0 -<opts> <customID> <project name> [Mainboard pathname]"
	echo -e "Notice: Make sure current path at \"<Smallboard path>/jrd_oem$\""
	echo -e "Default: Mainboard pathname = QCA9531"
	echo -e "Description: This shell script must be used after version Self-test completion\n"
	echo "Opts:"
	echo "-a  add -> commit -> package -> swver (Recommend)"
	echo "-v  version number add into release_history.txt"
	echo "-c  commit git store release_history.txt"
	echo "-p  package make"
	echo "-s  scp pacakge to swver@10.129.61.36"
	echo -e "-h  help\n"
	exit 1
}

while getopts ":avcpsh" opt
do
	case "$opt" in
		"a")
				echo "add version -> commit -> make package -> scp to swver"
				ver_value="yes"
				commit_value="yes"
				package_value="yes"
				scp_value="yes";;
		"v")
				echo "add version number to release_history.txt"
				ver_value="yes";;
		"c")
				echo "git commit release_history.txt"
				commit_value="yes";;
		"p")
				echo "make package"
				package_value="yes";;
		"s")
				echo "scp package to swver@10.129.61.36"
				scp_value="yes";;
		"h")
				help;;
		*)
				help;;
	esac
done

function get_ready_to_update ()
{
	echo "param \$1: $1"
	if [ $1 = $Small_board ]; then
		release_dir=$Small_board_path"/release_history"
		release_history_txt=$release_dir"/release_history.txt"
		board_version_info_file=$Small_board_path"/jrd_resource_dir/jrd-resource/resource/jrdcfg/jrd_version"
		gerrit_push_py=$Small_board_path"/gerrit-push.py"
		cd $Small_board_path
	elif [ $1 = $Main_board ]; then
		release_dir=$Main_board_path"/release_history"
		release_history_txt=$release_dir"/release_history.txt"
		board_version_info_file=$Main_board_path"/current_ver_info"
		gerrit_push_py=$Main_board_path"/gerrit-push.py"
		cd $Main_board_path
	else
		echo "Error, please check the param: $1"
	fi

#:<<!
	#for test:
	echo "For test:"
	echo "release_dir: $release_dir"
	echo "release_history_txt: $release_history_txt"
	echo "board_version_info_file: $board_version_info_file"
	echo "gerrit_push_py: $gerrit_push_py"
#!

	echo ">>>Start git pull release_history_file:"
    git fetch --all
    git reset --hard origin/master
    git pull

	#INNER_version:HH41_XX_02.00_0X
	current_verision_str=`sed -n 1p $board_version_info_file`
	echo "current_verision_str: $current_verision_str"

	if [ -n $current_verision_str ]; then
		#HH41_XX_02.00_0X
		current_version_info=`echo $current_verision_str | cut -d ':' -f 2`
		if grep -q $current_verision_str $release_history_txt
		then
			result=1
		else
			result=0
			echo "current_version_info is empty1..."
		fi
		echo "current_version_info: $current_version_info"
	else
		echo "current_version_info is empty2..."
	fi
}
	
function update_info_to_release_history_txt ()
{
	#get_ready_to_update $1
	echo "result: $result"

	echo ">>>Start update release_history.txt:"
	delimit_line="==============================================================================="
	if [ $result -eq 1 ]; then
		echo "Version number has [existed]. Needn't to update..."
	else
		line_cnt=2
		echo $delimit_line | sed -i "${line_cnt}i\\$delimit_line" $release_history_txt
		cat $board_version_info_file | while read line
		do
			line_cnt=$[ $line_cnt + 1 ]
			echo $line | sed -i "${line_cnt}i\\$line" $release_history_txt
			echo $line
		done
		echo "release_history.txt update finished..."
		return 0
	fi
	return 1
}

function git_commit_release_history_file ()
{
	echo ">>>Start git commit and gerrit push:"
	echo "please wait a moment..."
	git add $release_history_txt
	git commit -m "update $custom_name $1 release_history_file..."
	echo "y" | $gerrit_push_py
	echo "Y"
	echo ""
}

function get_custom_name_from_compile_info()
{
	custom_info_file=${PWD}"/HH41_ver/custom_info"
    if [ ! -f "$custom_info_file" ]; then
        echo "Could not find custom_info_file"
        exit 1
    fi

	#echo "custom_info_file: $custom_info_file"
	echo "customID: $customID"
	custom_name=`grep $customID HH41_ver/custom_info | cut -d '@' -f 1,2 | grep $customID | cut -d '@' -f 2`
    if [ $(echo $custom_name | wc -w) -ne 1 ]; then
        echo "Could not get custom name, please check custtom_info"
        exit 1
    fi
	echo "custom_name: $custom_name"
}

function make_package_dir_opt ()
{
	#XX_02.00_0X
	tmp_version_info=`echo $current_version_info | cut -d '_' -f 2,3-9`
	package_dir=`ls -rt | grep $tmp_version_info | tail -1`
	echo "tmp_version_info: $tmp_version_info"
	echo "package_dir: $package_dir"
	
	if [ $1 = $Small_board ]; then
		make_package_py=$Small_board_path"/makepackage.py"
		echo $make_package_py
	elif [ $1 = $Main_board ]; then
		make_package_py=$Main_board_path"/makepackage.py"
		echo $make_package_py
	else
		echo "Error, please check the param: $1"
	fi
	echo ">>>Start make package:"

	python $make_package_py $package_dir
}

function upload_package_to_swver ()
{
	cd $(dirname $(dirname ${PWD}))
	echo "cur_path: ${PWD}"

	package_file=`ls -rt | grep $custom_name -i | tail -1`
	echo "package_file: $package_file"
	echo ">>>Start copy $custom_name version package:"

	project_dir=$customID"_"$custom_name
	echo "project_dir: $project_dir"  #eg: JO_Claro_Dominica

	if [ "$project_name" = "HH40" ]; then
		target_path="HH40_HUB40/"${project_dir}
	elif [ "$project_name" = "HH41" ]; then
		target_path="HH41_HUB41/"${project_dir}
	else
		echo "$project_name error, please check."
	fi

	echo "target_path: $target_path"
	if ! sshpass -p swver ssh swver@10.129.61.36 "test -d /home/swver/sw_versions/${target_path}" ; then
		sshpass -p swver ssh swver@10.129.61.36 "mkdir -p /home/swver/sw_versions/${target_path}"
		echo ">>>Create /home/swver/sw_versions/${target_path} , Start uploading:"
	else
		echo ">>>Directory is existed, Start uploading:"
	fi

	sshpass -p swver scp -r $package_file swver@10.129.61.36:"/home/swver/sw_versions/${target_path}/"
	echo "Copy package success!!!"
	echo ">>>Start delete $package_dir local package directory..."
	rm $package_file -rf
	echo ">>>Delete $package_dir success!!!"
	echo ""
}

#Start here, help when command error
if [ $# -eq 4 ]; then
	echo "param: $1"
	customID=$2
	project_name=$3
	main_board_path_name=$4
	small_board_path_name=`echo ${PWD} | cut -d '/' -f 5`
	echo "small_board_path_name: $small_board_path_name"
	echo "main_board_path_name: $main_board_path_name"
	if [[ $main_board_path_name =~ "/" ]]; then
		echo "Error, Mainboard pathname can't include '/'..."
		exit 1
	fi
elif [ $# -eq 3 ]; then
	echo "param: $1"
	customID=$2
	project_name=$3
	main_board_path_name="QCA9531" # default
	small_board_path_name=`echo ${PWD} | cut -d '/' -f 5`
	echo "small_board_path_name: $small_board_path_name"
	echo "main_board_path_name: $main_board_path_name"
	if [[ $main_board_path_name =~ "/" ]]; then
		echo "Error, Mainboard pathname can't include '/'..."
		exit 2
	fi
else
	help
	exit 20
fi

Small_board_path=${PWD}
Main_board_path=$(dirname $(dirname ${PWD}))"/"$main_board_path_name"/jrd_oem"

get_custom_name_from_compile_info $customID

# -v -vc
if [ "$ver_value" = "yes" ]; then
	get_ready_to_update $Small_board
	update_info_to_release_history_txt #$Small_board
	if [ "$commit_value" = "yes" ]; then
		if grep -q $current_version_info $release_history_txt
		then
			echo ">>>Start git commit $Small_board:"
			git_commit_release_history_file $Small_board
		else
			echo "Needn't to git commit..."
		fi
	else
		exit 3
	fi

	get_ready_to_update $Main_board
	update_info_to_release_history_txt #$Main_board
	if [ "$commit_value" = "yes" ]; then
		if grep -q $current_version_info $release_history_txt
		then
			echo ">>>Start git commit $Main_board:"
			git_commit_release_history_file $Main_board
		else
			echo "Needn't to git commit..."
		fi
	else
		exit 4
	fi
else
	echo "pass ver_value"
fi

# -p 
# 如果单独打包使用-p参数时需要打开下面的get_ready_to_update
# 或者直接使用makepackage.py打包
if [ "$package_value" = "yes" ]; then
	#get_ready_to_update $Small_board
	make_package_dir_opt $Small_board

	#get_ready_to_update $Main_board
	make_package_dir_opt $Main_board
else
	echo "pass package_value"
fi

# -s
if [ "$scp_value" = "yes" ]; then
	upload_package_to_swver
	echo "upload success..."
else
	echo "pass scp_value"
fi

echo -e "\nAll work done!!!"
exit 0
