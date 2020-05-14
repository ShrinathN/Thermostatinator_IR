#!/bin/python
import tkinter as tk
import pandas as pd
import socket
import threading
import sys
import math
import time

array_raw = []
array_volt = []
array_resistance = []
array_temp = []
array_timestamp = []

beta = 3984

FONT_SIZE_SMOL = 30
FONT_SIZE_BIGU = 50

TO_SEND = ''

def savefunc():
	df = pd.DataFrame({
		'array_raw' : array_raw,
		'array_volt' : array_volt,
		'array_resistance' : array_resistance,
		'array_temp' : array_temp,
		'array_timestamp' : array_timestamp
	})
	df.to_csv('out.csv')

def turn_on():
	global TO_SEND
	TO_SEND = 1

def turn_off():
	global TO_SEND
	TO_SEND = 0

def fix_endian(x):
	return (x[3] << 24) | (x[2] << 16) | (x[1] << 8) | x[0]


def data_puller():
	global TO_SEND
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR | socket.SO_REUSEPORT, 0)
	s.connect((sys.argv[1], 8000))
	while(True):
		data = s.recv(4)

		if(type(TO_SEND) == type(int())):
			s.send(TO_SEND)
			print('Sending! ' + str(TO_SEND) + '\n')
			TO_SEND = ''


		raw_data = fix_endian(data)
		volt_data = (raw_data / 1024) * 3.3
		resistance_data = ((3.3 * 1000) / volt_data) - 1000


		r_inf = 10000 * math.exp(-beta/298.15)
		temp_data = (beta / math.log(resistance_data / r_inf, math.e)) - 273.15 - 1.5

		#adding to db
		array_raw.append(str(round(raw_data, 3)))
		array_volt.append(str(round(volt_data, 3)))
		array_resistance.append(str(round(resistance_data, 3)))
		array_temp.append(str(round(temp_data - 273.15, 3)))
		array_timestamp.append(time.ctime())

		#setting data
		raw_val['text'] = str(round(raw_data, 3))
		vlt_val['text'] = str(round(volt_data, 3))
		res_val['text'] = str(round(resistance_data, 3))
		tmp_val['text'] = str(round(temp_data, 3))

root = tk.Tk()
root.title('Temperature Data Recorder')

#labels
tk.Label(root, font=('',FONT_SIZE_SMOL), text="Raw Value").grid(row=0, column=0)
tk.Label(root, font=('',FONT_SIZE_SMOL), text="Voltage").grid(row=1, column=0)
tk.Label(root, font=('',FONT_SIZE_SMOL), text="Resistance").grid(row=2, column=0)
tk.Label(root, font=('',FONT_SIZE_SMOL), text="Temperature").grid(row=3, column=0)

#output
raw_val = tk.Label(root, font=('', FONT_SIZE_BIGU))
vlt_val = tk.Label(root, font=('', FONT_SIZE_BIGU))
res_val = tk.Label(root, font=('', FONT_SIZE_BIGU))
tmp_val = tk.Label(root, font=('', FONT_SIZE_BIGU))
raw_val.grid(row=0, column=1)
vlt_val.grid(row=1, column=1)
res_val.grid(row=2, column=1)
tmp_val.grid(row=3, column=1)

tk.Button(root, text='Save', command=savefunc).grid(row=4, column=0)
tk.Button(root, text='ON', command=turn_on).grid(row=4, column=1)
tk.Button(root, text='OFF', command=turn_off).grid(row=4, column=2)

threading.Thread(target=data_puller).start()

root.mainloop()
