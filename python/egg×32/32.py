# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-18 15:25:23
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-18 15:25:41

# 引入 datetime 模块
import datetime
def getYesterday(): 
    today=datetime.date.today() 
    oneday=datetime.timedelta(days=1) 
    yesterday=today-oneday
    return yesterday

# 输出
print(getYesterday())