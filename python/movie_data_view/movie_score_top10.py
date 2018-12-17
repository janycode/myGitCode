from pyecharts import Bar
import pandas as pd
import numpy as np
import pymysql

conn = pymysql.connect(host='localhost', user='root', password='774110919', port=3306, db='maoyan', charset='utf8mb4')
cursor = conn.cursor()
sql = "select * from films"
db = pd.read_sql(sql, conn)
df = db.sort_values(by="score", ascending=False)
dom = df[['name', 'score']]

v1 = []
for i in dom['score'][0:10]:
    number = float(i.replace('分', ''))
    v1.append(number)
attr = np.array(dom['name'][0:10])
attr = ["{}".format(i.replace('：致命守护者', '')) for i in attr]

bar = Bar("2018年电影评分TOP10", title_pos='center', title_top='18', width=800, height=400)
bar.add("", attr, v1, is_convert=True, xaxis_min=8, xaxis_max=9.8, yaxis_label_textsize=10, is_yaxis_boundarygap=True, yaxis_interval=0, is_label_show=True, is_legend_show=False, label_pos='right', is_yaxis_inverse=True, is_splitline_show=False)
bar.render("2018年电影评分TOP10.html")
