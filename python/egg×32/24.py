# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 15:22:19
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 15:22:21

def hcf(x, y):
  """该函数返回两个数的最大公约数"""

  # 获取最小值
  if x > y:
    smaller = y
  else:
    smaller = x

  for i in range(1, smaller + 1):
    if ((x % i == 0) and (y % i == 0)):
      hcf = i

  return hcf

# 用户输入两个数字
num1 = int(input("输入第一个数字: "))
num2 = int(input("输入第二个数字: "))

print(num1, "和", num2, "的最大公约数为", hcf(num1, num2))