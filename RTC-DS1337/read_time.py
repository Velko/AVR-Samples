#!/usr/bin/python3

import sys
import serial
import time

ser = serial.Serial("/dev/ttyUSB0", 115200)

try:
    while True:
        line = ser.readline().decode('ascii').strip()
        ltime = time.strftime("%Y-%m-%d %w %H:%M:%S", time.localtime())
        print (line, ltime)
except KeyboardInterrupt:
    pass
