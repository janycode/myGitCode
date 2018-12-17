from pyecharts import Bar
import pandas as pd
import numpy as np
import pymysql

conn = pymysql.connect(host='localhost', user='root', password='774110919', port=3306, db='maoyan', charset='utf8mb4')
cursor = conn.cursor()
sql = "select * from films"
db = pd.read_sql(sql, conn)
df = db.sort_values(by="released", ascending=False)
dom = df[['name', 'released']]
list1 = []
for i in dom['released']:
    place = i.split('-')[1]
    list1.append(place)
db['month'] = list1

month_message = db.groupby(['month'])
month_com = month_message['month'].agg(['count'])
month_com.reset_index(inplace=True)
month_com_last = month_com.sort_index()

attr = ["{}".format(str(i) + '月') for i in range(1, 12)]
v1 = np.array(month_com_last['count'])
v1 = ["{}".format(i) for i in v1]

bar = Bar("2018年每月上映电影数量", title_pos='center', title_top='18', width=800, height=400)
bar.add("", attr, v1, is_stack=True, yaxis_max=40, is_label_show=True)
bar.render("2018年每月上映电影数量.html")
