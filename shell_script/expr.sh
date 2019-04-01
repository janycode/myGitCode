#!/bin/bash


str1="09"
str2="08"
num1=`expr ${str1}`
num2=`expr ${str2}`

#let num3=$num1+$num2

if [ ${num1} -ge ${num2} ]; then
	echo "more"
	echo "num3: ${num3}"
else
	echo "less"
fi
