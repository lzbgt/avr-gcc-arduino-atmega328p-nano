# author Bruce.lu(lzbgt@icloud.com)

AVR_HOME:="C:/Program Files (x86)/Arduino/hardware/tools/avr"
ifeq ($(OS),Windows_NT)
	export PATH:="$(AVR_HOME)/bin";$(PATH)
else
	export PATH:="$(AVR_HOME)/bin":$(PATH)
endif

CC:=avr-gcc
CFLAGS:=-Os -DF_CPU=16000000UL -mmcu=atmega328p
PORT:=COM3 # /dev/ttyXXX for linux or macosx

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
		# -b 57600 must be specified for the old bootloader
		# -b 115200 for new bootloader (or let it choose automatically)
		avrdude -F -V -C $(AVR_HOME)/etc/avrdude.conf -c arduino -p m328p  -b 57600 -P $(PORT) -U flash:w:main.hex

main.hex: a.out
		avr-objcopy -O ihex -R .eeprom a.out main.hex

a.out: $(objects) Makefile
		$(CC) $(CFLAGS) $(objects) -o a.out

.PHONY: clean
clean:
		rm -f *.o *.out main *.hex
