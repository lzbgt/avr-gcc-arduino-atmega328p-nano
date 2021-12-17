# coding AVR with avr-gcc

## Makefile

```makefile
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

flash: main.hex
		echo "$(PATH)"
		# -C for specifing the configuration file
		# -c for programmer i.g SKII, STK500 ISP, here uses arduino
		# -p for target chip model, avr-gcc --target-help for more info
		# 	 here we use 'arduino nano' which is m328p
		# -b for searial boundrate, search keyword 'nano' in file 'Arduino/hardware/arduino/avr/boards.txt'
		avrdude -F -V -C $(AVR_HOME)/etc/avrdude.conf -c arduino -p m328p -P $(PORT) -b 57600  -U flash:w:main.hex

main.hex: a.out
		avr-objcopy -O ihex -R .eeprom a.out main.hex

a.out: $(objects)
		$(CC) $(CFLAGS) $(objects) -o a.out

clean:
		rm -f *.o *.out main *.hex

```

## AVR vs STM32

1. STM32 can do all things AVR chips (atmega 328 etc) can do, and do much more what AVR can't
2. AVR chip runs 1.7 ~ 5.0 V, but STM32 needs 3.3V LDO
3. AVR is better than STM32 in electrostatic discharge (ESD)
4. AVR ADC is better than STM32

It depends on the application itself to choose AVR or STM32

Both do their jobs good.
