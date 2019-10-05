PROGRAMMER=usbasp
TARGET=blinker
OBJS=main.o

CC=/usr/bin/avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -fshort-enums -mmcu=attiny45
OBJ2HEX=/usr/bin/avr-objcopy
DUDE=/usr/bin/avrdude

.SECONDARY: $(TARGET).elf

all: $(TARGET).hex

program : $(TARGET).hex
	$(DUDE) -p t45 -c $(PROGRAMMER) -y -U flash:w:$(TARGET).hex

fuses:
	# Turn off CKDIV8 fuse bit - enabling full speed on internal oscillator
	# We need for USBASP to slow down this time. Fresh, unconfigured ATTiny
	# runs too slow for default programming speed.
	$(DUDE) -p t45 -c $(PROGRAMMER) -B 3 -U lfuse:w:0xE2:m

%.elf : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.hex : %.elf
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.elf *.o *~
