#
# Makefile for UART example
#
# NOTE: edit F_OSC and DUDPORT to support either directly programming
# the hardware using a programming cable, or Arduino bootloader.
#

#----------------------------------------
# configure below for target
#----------------------------------------

#---------- Arduino ----------          
# MCU = atmega328
# F_OSC = 16000000
# EXTRAOPT = -DUNO_PINS
# DUDEMCU = m328p
# DUDEPORT  = -c arduino -P /dev/ttyACM0

#---------- FMC Target ----------
MCU = atmega88
F_OSC = 8000000
# define V21 for 2.1 laser board (swap IMON, PMON by default)
EXTRAOPT = -DFMC_PINS -DV21
DUDEMCU = m88
# USBTiny programmer from Amazon
DUDEPORT = -c usbtiny -p usb


# Alternative programmer: for AVRISP programmer on USB
# DUDEPORT  = -c avrispv2 -P usb


#----------------------------------------


CFLAGS = -g -Os -mmcu=$(MCU) $(EXTRAOPT) -DF_CPU=$(F_OSC) -Wl,-u,vfprintf -lprintf_min -lm
# no-optimization settings for interrupt code
CFLAGS_NOOPT = -g -O0 -mmcu=$(MCU) $(EXTRAOPT) -DF_CPU=$(F_OSC) -I../util -Wl,-u,vfprintf -lprintf_min

LDFLAGS = -Wl,-Map=$(<:.o=.map),--cref -mmcu=$(MCU)

# Disable clock divide-by-8
# (default ATMega88 LFUSE is 0x62)
FUSEOPT = -U lfuse:w:0xe2:m

# programmer configuration for avrdude programming utility

AVRDUDE  = avrdude $(DUDEPORT) -p $(DUDEMCU) -y -u 

objects = main.o uart.o ../libs/ioport.o i2c_wiki.o parse.o laser_io.o format_mv.o

headers = uart.h ../libs/ioport.h i2c_wiki.h parse.h format_mv.h

all: $(objects)  main.hex list

clean:
	rm -f *.o *~ *.hex *.elf *.map *.lst

flash: main.hex
	$(AVRDUDE) -U flash:w:$^:i

list:
	avr-objdump -h -S main.elf > main.lst
	avr-size --common -x main.elf

$(objects): %.o: %.c $(headers)
	avr-gcc -c $(CFLAGS) $< -o $@

main.elf: $(objects) $(cpp_objects)
	avr-gcc $(LDFLAGS) $(objects) -o $@

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

fuse:
	$(AVRDUDE) $(FUSEOPT)
