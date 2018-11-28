#!/usr/bin/env python

import gzip
f = open('hello.txt.gz', 'rb')
with gzip.open(f, 'rt') as g:
    text = g.read()
    print (text)
