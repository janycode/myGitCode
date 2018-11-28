#!/usr/bin/env python

def urlprint(protocol, host, domain):
	url = '{}://{}.{}'.format(protocol, host, domain)
	print(url)
