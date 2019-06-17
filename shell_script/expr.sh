#!/bin/bash


str1="09"
str2="08"
num1=`expr ${str1}`
num2=`expr ${str2}`

#num3=let $num1+$num2
#((num3=$num1+$num2))

if [ 50 -ge ${num2} ]; then
	echo "more"
	echo $(($num1+$num2)) 
else
	echo "less"
fi
