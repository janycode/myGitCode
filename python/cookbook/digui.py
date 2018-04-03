#!/usr/bin/env python3

list = [1, 10, 4, 7, 5, 9]

def sum(items):
	head, *tail = items
	if tail:
		return head + sum(tail)
	else:
		return head

print (sum(list))
