#!/bin/bash
# file name: [upload_to_swver.sh]
# file function: make package, copy package directory to swver, version number update, git commit files.
# file usage: $0 <customID> <project name> <Mainboard pathname>
# author: yuan.jiang

#set -v on

Small_board="small"
Main_board="main"

Small_board_path=${PWD}
Main_board_path=$(dirname $(dirname ${PWD}))"/"$3"/jrd_oem"

function help ()
{
    echo "Usage: $0 <customID> <project name> <Mainboard pathname>"
    echo "Make sure the current path is at <Small board>/jrd_oem"
    exit 1
}

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

    echo ">>>Start git pull:"
    git pull

    #INNER_version:HH41_XX_02.00_0X
    current_verision_str=`sed -n 1p $board_version_info_file`
    echo "current_verision_str: $current_verision_str"

    if [ -n $current_verision_str ]; then
        #HH41_XX_02.00_0X
        current_version_info=`echo $current_verision_str | cut -d ':' -f 2`
        if grep -q "$current_verision_str" $release_history_txt ; then
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
    get_ready_to_update $1
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
    git commit -m "update release_history_file..."
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

    echo "custom_info_file: $custom_info_file"
    custom_name=`grep $customID $custom_info_file | cut -d '@' -f 2`
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
    package_dir=`ls | grep "$tmp_version_info"`
    echo "tmp_version_info: $tmp_version_info"
    echo "package_dir: $package_dir"

    if [ $1 = $Small_board ]; then
        make_package_py=$Small_board_path"/makepackage.py"
    elif [ $1 = $Main_board ]; then
        make_package_py=$Main_board_path"/makepackage.py"
    else
        echo "Error, please check the param: $1"
    fi
    echo "make_package_py: $make_package_py"

    $make_package_py $package_dir
}

function upload_package_to_swver ()
{
    cd $(dirname $(dirname ${PWD}))
    echo "cur_path: ${PWD}"

    package_file=`ls | grep $custom_name -i`
    echo "package_file: $package_file"
    echo ">>>Start copy $custom_name version package..."

    project_dir=$customID"_"$custom_name
    echo $project_dir  #eg: JO_Claro_Dominica

    if [ $project_name = "HH40" ]; then
        target_path="HH40_HUB40/"${project_dir}
    elif [ $project_name = "HH41" ]; then
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

#--------------------------------------------------------------------#
#0. Start here, help when command error
if [ $# -eq 3 ]; then
    customID=$1
    project_name=$2
    main_board_path_name=$3
    echo "main_board_path_name:$main_board_path_name"
    if [[ $main_board_path_name =~ "/" ]]; then
        echo "Error, Mainboard pathname can't include '/'..."
        exit 1
    fi
else
    help
    exit 2
fi
#--------------------------------------------------------------------#

#--------------------------------------------------------------------#
#1. After compile, update and commit
get_custom_name_from_compile_info

update_info_to_release_history_txt $Small_board
if [ $? -eq 0 ]; then
    git_commit_release_history_file
    echo "git commit success..."
else
    echo "Needn't to git commit..."
fi

update_info_to_release_history_txt $Main_board
if [ $? -eq 0 ]; then
    git_commit_release_history_file
    echo "git commit success..."
else
    echo "Needn't to git commit..."
fi

echo "update and commit success..."
#--------------------------------------------------------------------#

#--------------------------------------------------------------------#
#2. Make packages, copy to swver
make_package_dir_opt $Small_board
make_package_dir_opt $Main_board
echo "make_package_dir_opt success..."
#--------------------------------------------------------------------#

#--------------------------------------------------------------------#
#3. Use scp to upload packages to swver
upload_package_to_swver
echo "upload_package_to_swver success..."
#--------------------------------------------------------------------#

echo ""
echo "All work done!!!"
exit 0
