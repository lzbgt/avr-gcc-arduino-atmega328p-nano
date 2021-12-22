# author Bruce.lu(lzbgt@icloud.com)

ifeq ($(OS),Windows_NT)
	AVR_HOME?="C:/Program Files (x86)/Arduino/hardware/tools/avr"
	export PATH:="$(AVR_HOME)/bin";$(PATH)
	PORT:=COM4 # /dev/ttyXXX for linux or macosx
else
	AVR_HOME?=/Applications/Arduino.app/Contents/Java/hardware/tools/avr
	export PATH:=$(PATH):$(AVR_HOME)/bin
	PORT:=/dev/tty.usbserial-1420
endif


CC:=$(AVR_HOME)/bin/avr-gcc
CFLAGS:=-Os -DF_CPU=16000000UL -mmcu=atmega328p


# rule for compiling all .c
objects := $(patsubst %.c,%.o,$(wildcard *.c))

# %.o: %.c
# 	$(CC) -Os -DF_CPU=16000000UL -mmcu=atmega328p -c $<

all: a.out main.hex

flash: main.hex
		# -C for specifing the configuration file
		# -c for programmer i.g SKII, STK500 ISP, here uses arduino
		# -p for target chip model, avr-gcc --target-help for more info
		# 	 here we use 'arduino nano' which is m328p
		# -b for searial boundrate, search keyword 'nano' in file 'Arduino/hardware/arduino/avr/boards.txt'

		$(AVR_HOME)/bin/avrdude -F -V -C $(AVR_HOME)/etc/avrdude.conf -c arduino -p m328p -P $(PORT) -b 57600  -U flash:w:main.hex

main.hex: a.out
		$(AVR_HOME)/bin/avr-objcopy -O ihex -R .eeprom a.out main.hex

a.out: $(objects) Makefile
		$(CC) $(CFLAGS) $(objects) -o a.out

.PHONY: clean
clean:
		rm -f *.o *.out main *.hex
