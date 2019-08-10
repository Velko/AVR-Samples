Introduction
============

This is an example project, how to connect AVR ATmega8 to DS1337 Real Time Clock chip. The device
reads current time from RTC chip each second and reports it via UART. There's also a routine for
setting the current time.

Hardware
========

The hardware consists from DS1337 chip connected via I2C bus to ATmega8 microcontroller. Additionally
RTC chip's INTA pin should be connected to ATmega's INT1. In the current configuration ATmega operates
using 7.3728 MHz crystal oscillator ("VelkoDuino" board).

Build and install
=================

Project is built using [AVR-GCC][avr-gcc] compilator toolchain. Installation to the ATmega8 utilizes
[AVRDUDE][avrdude] and [USBasp][usbasp] programming hardware.

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

Restart ATmega:
```bash
make reset
```

Set up fuses:
```bash
make fuses
```

Set and read time
=================

There are additional Python scripts included for setting and reading current time from the device. Current
configuration is using `/dev/ttyUSB0` for communication.

set_time.py
-----------
Waits for the precise moment when seconds change on the computer and then sends the SET TIME packet
to the device.

read_time.py
------------
Runs in a loop and prints the time received from the device, along with computer's time.



[avr-gcc]: https://gcc.gnu.org/wiki/avr-gcc
[avrdude]: https://www.nongnu.org/avrdude/
[usbasp]: https://www.fischl.de/usbasp/