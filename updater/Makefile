# Name: Makefile
# Project: USBaspLoader (updater)
# Author: Stephan Bärwolf
# Creation Date: 2012-09-01
# Tabsize: 4
# License: GNU GPL v2 (see License.txt)

include ../Makefile.inc

# elsewise gcc would complain unnecessary
CFLAGS = -Wall -Wno-pointer-to-int-cast -Os -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions -I. -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -DNEW_BOOTLOADER_ADDRESS=$(NEW_BOOTLOADER_ADDRESS) $(DEFINES)
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

../firmware/main.bin:
	$(MAKE) -C ../firmware main.hex

updater.o: updater.c firmware.h
	$(CC) updater.c -c -o updater.o $(CFLAGS)

updater.elf: updater.o
	$(CC) updater.o -o updater.elf $(CFLAGS) $(LDFLAGS)

updater.hex: updater.elf
	$(OBC) -j .text -j .data -O ihex updater.elf updater.hex
	$(ECHO) "."
	$(SIZ) updater.elf
	$(ECHO) "."
	$(AVRDUDE) -D -U flash:w:updater.hex:i
	$(ECHO) "."



firmware_gen.o: firmware_gen.c
	$(GCC) firmware_gen.c -c -o firmware_gen.o

firmware_gen: firmware_gen.o
	$(GCC) firmware_gen.o -o firmware_gen

firmware.h: firmware_gen ../firmware/main.bin
	$(OBC) -j .text -j .data -O binary ../firmware/main.bin usbasploader.raw
	@./firmware_gen > firmware.h


deepclean: clean
	$(RM) *~

clean:
	$(RM) *.o
	$(RM) *.raw
	$(RM) updater.hex
	$(RM) updater.elf
	$(RM) firmware_gen
	$(RM) firmware.h