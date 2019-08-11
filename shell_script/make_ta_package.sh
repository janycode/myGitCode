#!/bin/bash
# Author: yuan.jiang
# Data: 2018.08.24
# Description: Make package for TMC version(TA)

function help ()
{
	echo -e "Usage: $0 [version dir name] [PTS number] [CU ref]"
	echo -e "eg:"
	echo -e "\t./make_ta_package.sh Claro_Dominica_HH41NH_JO_02.00_03_20180317 042 HH41NH-2BAVDO1-1"
	exit 1
}

while getopts ":h" opt
do
	case "$opt" in
		"h")
				help;;
		*)
				help;;
	esac
done

function create_ta_package_dir ()
{
	echo ">>> create_ta_package_dir"
	ta_package=$custom_name"_"$project_name$variant_name"_"$customID"_"$fixed_version"_"$version_num"_P"$pts_num"_"$date_str"_"$cu_ref
	echo "pts_num:          $pts_num"
	echo "cu_ref:           $cu_ref"
	echo "ta_package:       $ta_package"

	cd $target_path
	if [ -d "$ta_package" ]; then
		echo "TA package dir is existed. Please check..."
		exit 3
	else
		mkdir $ta_package
		echo "TA package dir is created..."
	fi

	echo ""
}

function copy_files_and_unzip ()
{
	echo ">>> copy_files_and_unzip"

	cp $version_path/* $ta_package -a
	cd $ta_package
	
	ver_config_xml=$version_path"/config.xml"
	if [ ! -f $ver_config_xml ]; then
		testmd5_file="testmd5.MD5"
		config_xml_file="config.xml"
		fotaImage_config_xml="FotaImage/config.xml"
		cp $fotaImage_config_xml $config_xml_file
		
		config_xml_md5=`md5sum $config_xml_file | tr a-z A-Z | cut -d " " -f 1`
		config_xml_md5_str=$config_xml_md5"\tconfig.xml"
		echo "config.xml md5: $config_xml_md5"

		echo -e $config_xml_md5_str >> $testmd5_file
	fi
	
	if [ -d "*board" ]; then
		rm *board -rf
	fi

	zip_file=$version_str".zip"
	unzip $zip_file
	if [ -f "$zip_file" ]; then
		rm *.zip
	else
		echo "It has no .zip at here..."
	fi
	echo ""
}

function touch_pts_txt ()
{
	echo ">>> touch_pts_txt"
	
	pts_file="PTS"$pts_num".txt"
	echo "$pts_num" >$pts_file
	if [ -f "$pts_file" ]; then
		echo "PTS file is created..."
	else
		echo "PTS file created fail..."
	fi
	echo ""
}

function add_simlock_config ()
{
	echo ">>> add_simlock_config"

	factory_db=$version_str"/FotaImage/jan_resource/resource/sqlite3/factory_info.db3"
	simlock_plmns=`sqlite3 $factory_db "select * from sim_config;" | grep "NetworkCode" | cut -d '|' -f 2`
	echo "simlock_plmns: $simlock_plmns"

	if [ "$simlock_plmns" == "00000" ]; then
		echo "No SIMLOCK version!!!"
		simlock_flag=0
	else
		echo "SIMLOCK version!!!"
		simlock_flag=1
	fi

	if [ $simlock_flag == 1 ]; then
		#echo ${PWD}
		simlock_cfg_file="Simlock/simlock.cfg"
		mkdir Simlock
		touch Simlock/simlock.cfg

		echo -e "#"$ta_package > $simlock_cfg_file
		echo -e "[CODE]" >> $simlock_cfg_file
		echo -e "PN="$simlock_plmns >> $simlock_cfg_file
		echo -e "PU=NULL" >> $simlock_cfg_file
		echo -e "PP=NULL" >> $simlock_cfg_file
		echo -e "PC=NULL" >> $simlock_cfg_file
		echo -e "PF=NULL" >> $simlock_cfg_file
		echo -e "" >> $simlock_cfg_file
		echo -e "[ACTION]" >> $simlock_cfg_file
		echo -e "PN=2" >> $simlock_cfg_file
		echo -e "PU=0" >> $simlock_cfg_file
		echo -e "PP=0" >> $simlock_cfg_file
		echo -e "PC=0" >> $simlock_cfg_file
		echo -e "PF=0" >> $simlock_cfg_file
	fi

	echo ""
}

function wait_for_release_note_doc ()
{
	echo ">>> wait_for_release_note_doc"
	
	echo -e "\nNow, write modified points into \"Release Notes.doc\" ."
	while true
	do
		stty erase ^H # Enable Backspace key.
		read -p "Press [next] to continue:" next_str
		if [ "$next_str" == "next" ]; then
			break
		else
			echo "Please confirm Release Notes.doc is ready, and check your input..."
			continue
		fi
	done
	
	echo ""
}

function pack_ta_package ()
{
	echo ">>> pack_ta_package"

	echo "current path: ${PWD}"
	ta_package_path=${PWD}
	cd $(dirname ${PWD})

	echo "Start pack TA package .zip file:"
	ta_package_zip=$ta_package".zip"
	fota_image_dir=$ta_package"/FotaImage*"
	echo "fota_image_dir: $fota_image_dir"
	if [ -d "$ta_package" ]; then
		# Pack target dir except the dir specified by '-d'.
		echo
		echo "Packing..."
		zip -r $ta_package_zip $ta_package > /dev/null && zip -d $ta_package_zip "$fota_image_dir" > /dev/null && mv $ta_package_zip $ta_package
		
		cd $ta_package_path
		#echo "HERE >>> test path: $ta_package_path"
		rm *board -rf
		echo
		for ((i=0;$i<=100;i+=2))
		do
			if [ $i -eq 100 ]; then
				sleep 1
			fi
			printf ">>>[%-50s]%d%%\r" $b $i
			sleep 0.1
			b=#$b
		done
		echo
		echo "All work done!!!"
	else
		echo "Can't find TA package dir..."	
	fi
	
	echo ""
}

# eg: /home/swver/sw_versions/HH41_HUB41
current_path=${PWD}

# Start here:
if [ $# -eq 3 ]; then
	echo "param1: $1"
	echo "param2: $2"
	echo "current_path: $current_path"
	version_str=$1
	#customID=`echo $1 | cut -d '_' -f 4`
	#customID=`echo ${version_str:0-20:2}`
	customID=`echo $version_str | awk -F '_' '{print $(NF-3)}'`
	#custom_name=`echo $1 | cut -d '_' -f 1-2`
	custom_name=`ls | grep "$customID" | cut -d '_' -f 2-`
	#variant_name=`echo $1 | cut -d '_' -f 3 | cut -c 5-6`
	#tmp_variant_name=`echo ${version_str%_$custom_ID*}`
	#variant_name=`echo ${tmp_variant_name:0-2:2}`
	tmp_string=`echo $version_str | awk -F '_' '{print $(NF-4)}'`  #HH41NH / HH41V
	variant_name=`echo ${tmp_string:4:2}` #NH / V
	#project_name=`echo $1 | cut -d '_' -f 3 | cut -c 1-4`
	#tmp_project_name=`echo ${tmp_variant_name%$variant_name*}`
	#project_name=`echo ${tmp_project_name:0-4:4}`
	project_name=`echo ${tmp_string:0:4}` #HH41 / HH40
	#fixed_version=`echo $1 | cut -d '_' -f 5`
	#fixed_version=`echo ${version_str:0-17:5}`
	fixed_version=`echo $version_str | awk -F '_' '{print $(NF-2)}'`
	#version_num=`echo $1 | cut -d '_' -f 6`
	#version_num=`echo ${version_str:0-11:2}`
	version_num=`echo $version_str | awk -F '_' '{print $(NF-1)}'`
	#date_str=`echo $1 | cut -d '_' -f 7` 
	#date_str=`echo ${version_str:0-8:8}`
	date_str=`echo $version_str | awk -F '_' '{print $NF}'`
	target_path=$current_path"/"$customID"_"$custom_name
	version_path=$current_path"/"$customID"_"$custom_name"/"$1
	pts_num=$2
	cu_ref=$3
	#config_xml_path=$version_path"/FotaImage/config.xml"
	echo ""
	echo "version_str:      $version_str"
	echo "custom ID:        $customID"
	echo "custom_name:      $custom_name"
	echo "variant_name:     $variant_name"
	echo "project_name:     $project_name"
	echo "fixed_version:    $fixed_version"
	echo "version_num:      $version_num"
	echo "date_str:         $date_str"
	echo ""
	echo "target_path:      $target_path"
	echo "version_path:     $version_path"
	#echo "config_xml_path: $config_xml_path"
	echo ""
	#cu_ref=`grep 'CURef' $config_xml_path | cut -d '>' -f 2 | cut -d '<' -f 1`
else
	help
	exit 2
fi

create_ta_package_dir
copy_files_and_unzip
touch_pts_txt
#add_simlock_config
wait_for_release_note_doc
pack_ta_package
