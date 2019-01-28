# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 15:17:17
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 15:18:51


nterms = int(input('请输入一个数字：'))

# 判断输入的值是否合法
if nterms <= 0:
  print("请输入一个正整数。")
elif nterms == 1:
  print("斐波那契数列：")
  n1 = 1
  print(n1)
else:
  print("斐波那契数列：")
  print(n1, ",", n2, end=" , ")
  while count < nterms:
    nth = n1 + n2
    print(n1+n2, end=" , ")
    # 更新值
    n1 = n2
    n2 = nth
    count += 1