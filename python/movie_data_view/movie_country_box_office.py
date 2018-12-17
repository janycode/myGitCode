import pandas as pd
import numpy as np
import pymysql
from pyecharts import Pie

conn = pymysql.connect(host='localhost', user='root', password='774110919', port=3306, db='maoyan', charset='utf8mb4')
cursor = conn.cursor()
sql = "select * from films"
db = pd.read_sql(sql, conn)
list1 = []
for i in db['country']:
    type1 = i.split(',')[0]
    if type1 in ['中国大陆', '中国香港']:
        type1 = '中国'
    else:
        type1 = '外国'
    list1.append(type1)
db['country_type'] = list1

country_type_message = db.groupby(['country_type'])
country_type_com = country_type_message['box_office'].agg(['sum'])
country_type_com.reset_index(inplace=True)
country_type_com_last = country_type_com.sort_index()

attr = country_type_com_last['country_type']
v1 = np.array(country_type_com_last['sum'])
v1 = ["{}".format(float('%.2f' % (float(i) / 100000000))) for i in v1]

pie = Pie("2018年中外电影票房对比(亿元)", title_pos='center')
pie.add("", attr, v1, radius=[40, 75], label_pos='right', label_text_color=None, is_label_show=True, legend_orient="vertical", legend_pos="left",label_formatter='{c}')
pie.render("2018年中外电影票房对比(亿元).html")