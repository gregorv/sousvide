
CC=avr-gcc

all: sousvide.hex

flash: sousvide.hex
	avrdude -P /dev/parport0 -c stk200 -p m8 -e -U sousvide.hex

sousvide.hex: sousvide.elf
	avr-size sousvide.elf
	avr-objcopy -O ihex  sousvide.elf sousvide.hex
	# avr-objcopy --no-change-warnings -j .eeprom --change-section-lma .eeprom=0 -O ihex coffstat.elf coffstat.elf.eep.hex

clean:
	rm -f sousvide.elf sousvide.hex

sousvide.elf: main.c wifi.c
	$(CC) -o sousvide.elf -DF_CPU=20083200L -mmcu=atmega8 -O3 -Wall -lm -lprintf_flt -Wl,-u,vfprintf main.c wifi.c

