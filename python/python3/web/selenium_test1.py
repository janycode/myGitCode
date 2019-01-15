# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-14 10:42:45
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-14 13:21:33

from selenium import webdriver
import time

def login(username, password):
    driver = webdriver.Firefox()
    driver.get('https://ep.tclcom.com')
    print (browser.page_source)

    # userNameInput | passwordInput | submitButton
    user_input = driver.find_element_by_css_selector("#userNameInput")
    print ('user_input:', user_input)
    pass_input = driver.find_element_by_css_selector("#passwordInput")
    print ('pass_input:', pass_input)
    login_button = driver.find_element_by_css_selector("#submitButton")
    print ('login_button:', login_button)

    name_input.clear()
    name_input.send_keys(username)  # 填写用户名
    time.sleep(2)
    pass_input.clear()
    pass_input.send_keys(password)  # 填写密码
    time.sleep(2)
    login_button.click()            # 点击登录

    time.sleep(2)
    print (driver.get_cookies())

    time.sleep(2)
    print (driver.title)

    driver.close()

if __name__ == '__main__':
    usr = "ta-nj\yuan.jiang"
    pwd = "ggg@tcl123"
    login(usr, pwd)