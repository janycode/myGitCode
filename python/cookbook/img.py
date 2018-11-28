#!/usr/bin/env python
# coding=utf-8

from PIL import Image, ImageDraw, ImageFont
img = Image.open("./img/003.jpg")
jgz = Image.open("./img/002.jpg")
img.paste(jgz, (45,36))
#img.show()


draw = ImageDraw.Draw(img)
ttfront = ImageFont.truetype('simhei.ttf', 16)
draw.text((20, 190), "我的内心毫无波动甚至还想笑",fill=(0,0,0), font=ttfront)
img.show()
img.save("./img/result.jpg")

# 待处理文字长度与图片宽度的自动兼容