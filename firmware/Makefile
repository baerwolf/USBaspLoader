# Name: Makefile
# Project: USBaspLoader
# Author: Christian Starkjohann
# Creation Date: 2007-12-10
# Author: Stephan Bärwolf
# Improvement Date: 2012-07-31
# Tabsize: 4
# Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt)

###############################################################################
# Configure the following variables according to your AVR.
# Program the device with
#     make fuse    # to set the clock generator, boot section size etc.
#     make flash   # to load the boot loader into flash
#     make lock    # to protect the boot loader from overwriting

include ../Makefile.inc

# Remove the -fno-* options when you use gcc 3, it does not understand them
CFLAGS = -Wall -Os -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions -I. -mmcu=$(DEVICE) -DBOOTLOADER_ADDRESS=$(BOOTLOADER_ADDRESS) -DF_CPU=$(F_CPU) $(DEFINES)
LDFLAGS = -Wl,--relax,--gc-sections -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS) -Wl,--defsym=nullVector=0

OBJECTS =  usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o

# symbolic targets:
all: main.hex

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.S.o:
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(CC) $(CFLAGS) -S $< -o $@

flash:	all
	$(ECHO) "."
	$(AVRDUDE) -U flash:w:main.hex:i
	$(ECHO) "."
	$(ECHO) "."

readflash:
	$(ECHO) "."
	$(AVRDUDE) -U flash:r:read.hex:i
	$(ECHO) "."
	$(ECHO) "."


fuse:
	$(ECHO) "."
	$(AVRDUDE) $(FUSEOPT)
	$(ECHO) "."
	$(ECHO) "."


lock:
	$(ECHO) "."
	$(AVRDUDE) $(LOCKOPT)
	$(ECHO) "."
	$(ECHO) "."


read_fuses:
	$(UISP) --rd_fuses

deepclean: clean
	$(RM) *~

clean:
	$(RM) main.hex
	$(RM) main.bin
	$(RM) main.o
	$(RM) main.s
	$(RM) usbdrv/usbdrvasm.o
	$(RM) usbdrv/oddebug.o
	$(RM) usbdrv/oddebug.s
	$(RM) usbdrv/usbdrv.s

# file targets:
main.bin:	$(OBJECTS) bootloaderconfig.h
	$(CC) $(CFLAGS) -o main.bin $(OBJECTS) $(LDFLAGS)

main.hex:	main.bin
	$(RM) main.hex main.eep.hex
	$(OBC) -j .text -j .data -O ihex main.bin main.hex
	$(ECHO) "."
	$(ECHO) "."
	$(ECHO) "."
	$(ECHO) "!!!ATTANTION!!!"
	$(ECHO) "(data+text) MUST fit into your MCUs bootloader section"
	$(ECHO) "."
	$(SIZ) --mcu $(DEVICE) main.bin
	$(ECHO) "."
	$(ECHO) "."
	$(ECHO) "."

disasm:	main.bin
	$(OBD) -d main.bin

cpp:
	$(CC) $(CFLAGS) -E main.c
