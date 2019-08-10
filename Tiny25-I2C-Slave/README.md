Introduction
============

This is an example project demonstrating how to use ATtiny25 as a I2C slave device. The device
provides means of controlling single LED using commands sent via I2C bus. It is based on AVR312
app-note, but a number of improvements were made using my own programmer-feel and advices from
various sources. 

Honorable mentions: 
* [ATTiny USI I2C Introduction][i2c-instructable]
* [What's wrong with the AVR Appnote][whats-wrong]

Hardware
========

The hardware consists from ATtiny25 microcontroller, connected to Raspberry Pi. SDA and SCL pins
should be connected between both devices. The MCU should be powered from RPi's 3v3 pin or another
source with same voltage. The output LED is connected to ATtiny's PB1 pin in a active-high 
configuration (other lead to the GND). Obviously, 330 ohm or 1K series resistor should be used for
the LED.

Project is built using [AVR-GCC][avr-gcc] compilator toolchain. Installation to the ATmega8 utilizes
[AVRDUDE][avrdude] and [USBasp][usbasp] programming hardware.

Build and install
=================

Common operations
-----------------
Build:
```bash
make
```

Flash:
```bash
make program
```

Set up fuses:
```bash
make fuses
```

Raspberry Pi setup
------------------

I2C communications should be enabled on RPi. Use `sudo raspi-config` utility, Interfacing Option -> I2C.

The user should be added to `i2c` group. Add it to the group using (and re-login afterwards):
```bash
sudo adduser pi i2c
```

The test script requires Python3 and SMBUS library. Install those using:
```bash
sudo apt-get install python3 python3-smbus
```

Read and set LED state
======================

To control the device, use script `lampctrl.py`. Currently there are few commands available:
* **state** - read current state
* **on** - turn the LED on
* **off** - turn the LED off
* **toggle** - toggle the state

The device can also be controlled using standard i2c-tools, for example:
```
i2cset -y 1 0x30 0x00 0x00
i2cset -y 1 0x30 0x00 0xff
```

[i2c-instructable]: https://www.instructables.com/id/ATTiny-USI-I2C-The-detailed-in-depth-and-infor/
[whats-wrong]: http://www.aca-vogel.de/TINYUSII2C_AVR312/APN_TINYUSI_I2C.html