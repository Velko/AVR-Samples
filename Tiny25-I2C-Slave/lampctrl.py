#!/usr/bin/python3

import smbus, argparse

bus = smbus.SMBus(1)

class LampDevice:
    def __init__(self, hwid):
        self.hwid = hwid
        self.isOn = False

    def loadState(self):
        self.isOn = bus.read_byte_data(self.hwid, 0x0) != 0
        return self.isOn

    def setState(self, val):
        bus.write_byte_data(self.hwid, 0x0, 1 if val else 0)
        self.isOn = val


lamp = LampDevice(0x30)

if __name__ == "__main__":

    cmdparser = argparse.ArgumentParser()
    cmdparser.add_argument("action", choices=['state', 'on', 'off', 'toggle'])

    args = cmdparser.parse_args()

    if args.action == "state":
    	print ("on" if lamp.loadState() else "off")
    elif args.action == "on":
        lamp.setState(True)
    elif args.action == "off":
        lamp.setState(False)
    elif args.action == "toggle":
        lamp.setState(not lamp.loadState())

