#!/usr/bin/python3

import smbus

bus = smbus.SMBus(1)

class LampDevice:
    def __init__(self, hwid):
        self.hwid = hwid
        self.isOn = False

    def loadState(self):
    	self.isOn = bus.read_byte_data(self.hwid, 0x0) != 0

    def setState(self, val):
        bus.write_byte_data(self.hwid, 0x0, 1 if val else 0)
        self.isOn = val


lamp = LampDevice(0x30)

if __name__ == "__main__":
    lamp.loadState()

    print ("Lamp 1", lamp.isOn)

    lamp.setState(not lamp.isOn)

