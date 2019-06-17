#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Author: yuan-jiang
# @Date:   2019-03-06 18:16:16
# @Last Modified by:   yuan-jiang
# @Last Modified time: 2019-03-06 18:16:23

import tesserocr
from PIL import Image

image = Image.open('code.jpg')
result = tesserocr.image_to_text(image)
print (result)