#!/bin/python
import time
from socket import *

n = 0
while(True):
	s = socket(AF_INET, SOCK_STREAM, 0)
	s.connect(('192.168.100.20', 8000))
	n += 1
	print(s.recv(4), end='')
	print(n)
	s.shutdown(SHUT_RDWR)
	s.close()
	time.sleep(1)