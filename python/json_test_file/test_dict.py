#!/usr/bin/env python
#coding: utf-8

import json

jsonstring = '{"key":"value","obj":{"objkey":"objvalue"}}'
myobj = json.loads(jsonstring);
datas = json.dumps(myobj);

print type(myobj)           # <type 'dict'>
print type(myobj['key'])    # <type 'unicode'>
print type(myobj['obj'])    # <type 'dict'>
print type(datas)           # <type 'str'>
