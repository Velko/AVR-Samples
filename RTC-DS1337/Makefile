#
# Target's name
#
TARGET=clock

#
# Object files
#
OBJS=main.o uart.o twi.o rtc.o

#
# MCU name for GCC
#
MCU_GCC=atmega8

#
# MCU name for programming software
#
MCU_PROG=m8

# TARGET=atmega8    HFUSE=0xc9  LFUSE=0xef
HFUSE=0xc9
LFUSE=0xef

#
# Programming hardware and software
#
PROG_HW=usbasp
PROG_SW=/usr/bin/avrdude



CC=/usr/bin/avr-gcc
LD=/usr/bin/avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=$(MCU_GCC)
OBJ2HEX=/usr/bin/avr-objcopy


all: $(TARGET).hex

program: $(TARGET).hex
	$(PROG_SW) -p $(MCU_PROG) -c $(PROG_HW) -y -U flash:w:$(TARGET).hex

reset: $(TARGET).hex
	$(PROG_SW) -p $(MCU_PROG) -c $(PROG_HW)

fuses:
	$(PROG_SW) -p $(MCU_PROG) -c $(PROG_HW) -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

%.elf: $(OBJS)
	$(LD) $(CFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.hex: %.elf
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f $(TARGET).hex $(TARGET).elf $(OBJS)


