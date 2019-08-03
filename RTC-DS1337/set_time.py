#!/usr/bin/python3

import sys
import serial
import time

ser = serial.Serial("/dev/ttyUSB0", 115200)

prevs = time.strftime("t%y%m%d%w%H%M%S\n", time.localtime())

s = prevs

while s == prevs:
    s = time.strftime("t%y%m%d%w%H%M%S\n", time.localtime())

print (s)

ser.write(s.encode('ascii'))
ser.flush()
