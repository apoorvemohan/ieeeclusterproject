
def test(a):
	print 'test'  + str(a)

def test1(a):
	print 'test1' + str(a)


func = [test, test1]

import random
import sys

b = random.choice(func)

b(1)
