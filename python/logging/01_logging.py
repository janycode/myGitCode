#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-22 15:08:54
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-22 15:29:19
import logging

''' 配置日志器的日志信息 '''
# LOG_FORMAT = "%(asctime)s - %(levelname)s - %(message)s"
# 加入自定义字段user,ip
LOG_FORMAT = "%(asctime)s - %(levelname)s - %(user)s[%(ip)s] - %(message)s"
DATE_FORMAT = "%m/%d/%Y %H:%M:%S %p"
logging.basicConfig(filename='my.log', level=logging.DEBUG, format=LOG_FORMAT, datefmt=DATE_FORMAT)


logging.debug("This is a debug log.")
logging.info("This is a info log.")
logging.warning("This is a warning log.")
logging.error("This is a error log.")
logging.critical("This is a critical log.")

# 对应自定义字段user,ip
user_ip = {'user':'Jan', 'ip':'47.98.53.222'}
logging.warning("This is a warning log.", exc_info=True, stack_info=True, extra=user_ip)