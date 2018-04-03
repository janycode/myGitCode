#!/usr/bin/env python3
from operator import itemgetter
import pprint as pp

rows = [
	{'fname': 'Brian', 'lname': 'Jones', 'uid': 1003},
	{'fname': 'David', 'lname': 'Beazley', 'uid': 1002},
	{'fname': 'John', 'lname': 'Cleese', 'uid': 1001},
	{'fname': 'Big', 'lname': 'Jones', 'uid': 1004}
]

print (type(rows))

rows_by_name = sorted (rows, key=itemgetter('fname'))
rows_by_uid  = sorted (rows, key=itemgetter('uid'))

pp.pprint (rows_by_name)
pp.pprint (rows_by_uid)
