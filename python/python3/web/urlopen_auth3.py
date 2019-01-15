# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-11 14:27:56
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-11 14:34:37

import urllib.request as ur
import urllib.error as ue
import urllib.parse as up

LOGIN = 'wesley'
PASSWD = "you'llNeverGuess"
URL = 'http://localhost'
REALM = 'Secure Archive'

def handler_version(url):
    hdlr = ur.HTTPBasicAuthHandler()
    hdlr.add_password(REALM, up.urlparse(url)[1], LOGIN, PASSWD)
    opener = ur.build_opener(hdlr)
    ur.install_opener(opener)
    return url

def request_version(url):
    from base64 import encodestring
    req = ur.Request(url)
    b64str = encodestring(bytes('%s:%s' % (LOGIN, PASSWD), 'utf-8'))[:-1]
    req.add_header("Authorization", "Basic %s" % b64str)
    return req

for funcType in ('handler', 'request'):
    print ('*** Using %s:' % funcType.upper())
    url = eval('%s_version' % funcType)(URL)
    f = ur.urlopen(url)
    print (str(f.readline(), 'utf-8'))
    f.close()