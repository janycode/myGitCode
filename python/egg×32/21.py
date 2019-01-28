# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 15:16:15
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 15:16:31


N = int(input('输入需要对比大小的数字的个数：'))
print("请输入需要对比的数字：")
num = []
for i in range(1,N+1):
    temp = int(input('请输入第%d个数字：'%i))
    num.append(temp)
print('您输入的数字为：',num)
print('最大值为：',max(num))

N = int(input('输入需要对比大小的数字的个数：\n'))
num = [int(input('请输入第%d个数字：\n'%i))for i in range(1,N+1)]
print('您输入的数字为：',num)
print('最大值为：',max(num))