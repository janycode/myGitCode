from pyecharts import Bar
import pandas as pd
import numpy as np
import pymysql


def my_sum(a, b, c):
    rate = (a + b) / c
    result = float('%.4f' % rate)
    return result


conn = pymysql.connect(host='localhost', user='root', password='774110919', port=3306, db='maoyan', charset='utf8mb4')
cursor = conn.cursor()
sql = "select * from films"
db = pd.read_sql(sql, conn)
db['sort_num_money'] = db['box_office'].rank(ascending=0, method='dense')
db['sort_num_score'] = db['score'].rank(ascending=0, method='dense')
db['value'] = db.apply(lambda row: my_sum(row['sort_num_money'], row['sort_num_score'], len(db.index)), axis=1)
df = db.sort_values(by="value", ascending=True)[0:10]


v1 = ["{}".format('%.2f' % ((1-i) * 100)) for i in df['value']]
attr = np.array(df['name'])
attr = ["{}".format(i.replace('：无限战争', '').replace('：全面瓦解', '')) for i in attr]

bar = Bar("2018年电影名利双收TOP10(%)", title_pos='center', title_top='18', width=800, height=400)
bar.add("", attr, v1, is_convert=True, xaxis_min=85, xaxis_max=100, yaxis_label_textsize=12, is_yaxis_boundarygap=True, yaxis_interval=0, is_label_show=True, is_legend_show=False, label_pos='right', is_yaxis_inverse=True, is_splitline_show=False)
bar.render("2018年电影名利双收TOP10.html")
