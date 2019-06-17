#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Author: yuan-jiang
# @Date:   2019-03-04 15:04:15
# @Last Modified by:   yuan-jiang
# @Last Modified time: 2019-03-04 15:44:12

# 今日头条街拍图
# www.toutiao.com的搜索栏
# https://www.toutiao.com/search/?keyword=%E8%A1%97%E6%8B%8D
# RequestURL: https://www.toutiao.com/api/search/content/?aid=24&app_name=web_search&offset=0&format=json&keyword=%E8%A1%97%E6%8B%8D&autoload=true&count=20&en_qc=1&cur_tab=1&from=search_tab&pd=synthesis
# F12 >> XHR过滤 >> data >> title(文章标题)
import requests
from urllib.parse import urlencode
import os
from hashlib import md5
from multiprocessing.pool import Pool  # Pool线程池，多线程下载

keyword  = '新能源'
base_url = 'https://www.toutiao.com/api/search/content/?'

def get_page(offset):
    params = {
        'offset'  : offset,
        'format'  : 'json',
        'keyword' : keyword,
        'autoload': 'true',
        'count'   : '20',
        'cur_tab' : '1',
    }
    url = base_url + urlencode(params)
    try:
        response = requests.get(url)
        if response.status_code == 200:
            return response.json()
    except requests.ConnectionError:
        return None

def get_images(json):
    if json.get('data'):
        for item in json.get('data'):
            if item == None:
                break
            title = item.get('title')
            images = item.get('image_list')  #没有发现 image_detail 字段
            for image in images:
              yield {
                  'image' : image.get('url'),
                  'title' : title,
              }

def save_image(item):
    if not os.path.exists(item.get('title')):
        os.mkdir(item.get('title'))
    try:
        response = requests.get(item.get('image'))
        if response.status_code == 200:
            file_path = '{0}/{1}.{2}'.format(item.get('title'), md5(response.content).hexdigest(), 'jpg')
            if not os.path.exists(file_path):
                with open(file_path, 'wb') as f:
                    f.write(response.content)
            else:
                print ('Already Downloaded', file_path)
    except requests.ConnectionError:
        print ('Failed to Save Image.')

def main(offset):
    json = get_page(offset)
    for item in get_images(json):
        print (item)
        save_image(item)

GROUP_START = 1
GROUP_END   = 20

if __name__ == '__main__':
    pool = Pool()
    groups = ([x*20 for x in range(GROUP_START, GROUP_END+1)])
    pool.map(main, groups)
    pool.close()
    pool.join()