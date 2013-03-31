# Name: Makefile
# Project: USBaspLoader (updater)
# Author: Stephan Bärwolf
# Creation Date: 2012-09-01
# Tabsize: 4
# License: GNU GPL v2 (see License.txt)

include ../Makefile.inc

# elsewise gcc would complain unnecessary
CFLAGS = -Wall -Wno-pointer-to-int-cast -Os -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions -I. -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -DBOOTLOADER_ADDRESS=$(BOOTLOADER_ADDRESS) -DNEW_BOOTLOADER_ADDRESS=$(NEW_BOOTLOADER_ADDRESS) $(DEFINES)
LDFLAGS = -Wl,--relax,--gc-sections

ifneq ($(FLASHADDRESS), 0)
ifneq ($(FLASHADDRESS), 00)
ifneq ($(FLASHADDRESS), 000)
ifneq ($(FLASHADDRESS), 0000)
ifneq ($(FLASHADDRESS), 00000)
ifneq ($(FLASHADDRESS), 0x0)
ifneq ($(FLASHADDRESS), 0x00)
ifneq ($(FLASHADDRESS), 0x000)
ifneq ($(FLASHADDRESS), 0x0000)
ifneq ($(FLASHADDRESS), 0x00000)
FLASHPREAMBLE = 0x0000
LDFLAGS += -Wl,--section-start=.text=$(FLASHADDRESS)
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif

all:  updater.hex

flash:	all
	$(ECHO) "."
	$(AVRDUDE) -D -U flash:w:updater.hex:i
	$(ECHO) "."
	$(ECHO) "."


../firmware/main.bin:
	$(MAKE) -C ../firmware main.hex


usbasploader.raw: ../firmware/main.bin
	$(OBC) -j .text -j .data -O binary ../firmware/main.bin usbasploader.raw

usbasploader.o: usbasploader.raw
	$(OBC) -B $(MCUARCH) -I binary -O elf32-avr --rename-section .data=.text --redefine-sym _binary_usbasploader_raw_start=usbasploader  usbasploader.raw usbasploader.o


updater.o: updater.c usbasploader.h usbasploader.raw usbasploader.o
	$(CC) updater.c -c -o updater.o -DSIZEOF_new_firmware=$(shell stat -c %s usbasploader.raw) $(CFLAGS)
# 	$(CC) updater.c -c -o updater.o $(CFLAGS)

updater.elf: updater.o usbasploader.o
	$(CC) updater.o usbasploader.o -o updater.elf $(CFLAGS) $(LDFLAGS)

updater.hex: updater.elf
	$(OBC) -j .text -j .data -O ihex updater.elf updater.hex
	$(ECHO) "."
	$(ECHO) "."
	$(SIZ) updater.elf
	$(ECHO) "."

deepclean: clean
	$(RM) *~

clean:
	$(RM) usbasploader.o
	$(RM) updater.o
	$(RM) usbasploader.raw
	$(RM) updater.hex
	$(RM) updater.elf
	$(RM) usbasploader.raw