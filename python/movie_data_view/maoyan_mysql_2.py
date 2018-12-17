import pymysql

db = pymysql.connect(host='127.0.0.1', user='root', password='774110919', port=3306, db='maoyan')
cursor = db.cursor()
sql = 'CREATE TABLE IF NOT EXISTS films (name VARCHAR(255) NOT NULL, type VARCHAR(255) NOT NULL, country VARCHAR(255) NOT NULL, length VARCHAR(255) NOT NULL, released VARCHAR(255) NOT NULL, score VARCHAR(255) NOT NULL, people INT NOT NULL, box_office BIGINT NOT NULL, PRIMARY KEY (name))'
cursor.execute(sql)
db.close()
