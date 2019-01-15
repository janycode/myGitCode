#!/usr/bin/env python3

import datetime

year = datetime.datetime.now().year
now  = datetime.datetime.now().strftime("%y-%m-%d %H:%M:%S")
print ("This year is {}".format(year))
print ("Now  time is {}".format(now))