from pyecharts import Bar
import pandas as pd
import pymysql

conn = pymysql.connect(host='localhost', user='root', password='774110919', port=3306, db='maoyan', charset='utf8mb4')
cursor = conn.cursor()
sql = "select * from films"
db = pd.read_sql(sql, conn)
list1 = []
for i in db['country']:
    place = i.split(',')[0]
    list1.append(place)
db['location'] = list1

place_message = db.groupby(['location'])
place_com = place_message['location'].agg(['count'])
place_com.reset_index(inplace=True)
place_com_last = place_com.sort_index()
dom = place_com_last.sort_values('count', ascending=False)[0:10]

attr = dom['location']
v1 = dom['count']

bar = Bar("2018年各国家电影数量TOP10", title_pos='center', title_top='18', width=800, height=400)
bar.add("", attr, v1, is_stack=True, is_label_show=True)
bar.render("2018年各国家电影数量TOP10.html")
