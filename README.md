AVR Samples
===========

This is a collection of simple demo projects using AVR microcontrollers.
The purpose is to provide a simple firmware, that can be flashed to check
if the hardware is working as expected and to serve as a source of 
code-snippets for larger projects.

The projects are implemented with no dependencies from eachother. Also the
only external dependency is on [AVR Libc][avr-libc].


Contents
========

* RTC-DS1337 - use I2C interface to connect ATmega8 MCU with DS1337 RTC chip.
* Tiny25-I2C-Slave - Use Tiny25 USI as a I2C slave
* Uart-m328p - use USART0 on ATmega328p (Arduino Nano/Uno) as stdin/sdout

[avr-libc]: https://www.nongnu.org/avr-libc/
