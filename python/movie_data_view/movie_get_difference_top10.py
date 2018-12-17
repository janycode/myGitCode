from pyecharts import Bar
import pandas as pd
import numpy as np
import pymysql


def my_difference(a, b, c):
    rate = (a - b) / c
    return rate


conn = pymysql.connect(host='localhost', user='root', password='774110919', port=3306, db='maoyan', charset='utf8mb4')
cursor = conn.cursor()
sql = "select * from films"
a = pd.read_sql(sql, conn)
a['sort_num_money'] = a['box_office'].rank(ascending=0, method='dense')
a['sort_num_score'] = a['score'].rank(ascending=0, method='dense')
a['value'] = a.apply(lambda row: my_difference(row['sort_num_money'], row['sort_num_score'], len(a.index)), axis=1)
df = a.sort_values(by="value", ascending=True)[0:9]


v1 = ["{}".format('%.2f' % abs(i * 100)) for i in df['value']]
attr = np.array(df['name'])
attr = ["{}".format(i.replace('：无限战争', '')) for i in attr]

bar = Bar("2018年叫座不叫好电影TOP10", title_pos='center', title_top='18', width=800, height=400)
bar.add("", attr, v1, is_convert=True, xaxis_min=0, xaxis_max=4, yaxis_label_textsize=12, is_yaxis_boundarygap=True, yaxis_interval=0, is_label_show=True, is_legend_show=False, label_pos='right', is_yaxis_inverse=True, is_splitline_show=False)
bar.render("2018年叫座不叫好电影TOP100.html")
