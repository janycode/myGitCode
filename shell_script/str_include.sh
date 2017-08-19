#!/bin/bash
#string include

str_short="hello"
str_long="hello,shell!"

# grep查找法
function test1 ()
{
	echo -e "test1:\c"
	result=$(echo $str_long | grep "${str_short}")
	if [[ "$result" != "" ]]; then
		echo "包含"
	else
		echo "不包含"
	fi

}

# 字符串运算符
function test2 ()
{
	echo -e "test2:\c"
	if [[ $str_long =~ $str_short ]]; then
		echo "包含"
	else
		echo "不包含"
	fi
}

# 利用通配符
function test3 ()
{
	echo -e "test3:\c"
	if [[ $str_long == *$str_short* ]]; then
		echo "包含"
	else
		echo "不包含"
	fi
}

test1  #包含
test2  #包含
test3  #包含

exit 0



