#!/usr/bin/python
# coding=utf-8

print ("hello, python!")


with open("./test.txt", 'a+') as f:
	file_content = f.read()
	print (file_content)
	if "bbb" in file_content:
		print ("bbb is in file.")
	f.write("111")

f.close()
