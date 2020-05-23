#!/bin/python
import time
from socket import *
import sys

n = 0
while(True):
	s = socket(AF_INET, SOCK_STREAM, 0)
	s.connect((sys.argv[1], 8000))
	n += 1
	print(s.recv(4), end='')
	print(n)
	s.shutdown(SHUT_RDWR)
	s.close()
	time.sleep(1)