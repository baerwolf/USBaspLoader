# Name: Makefile.inc
# Project: USBaspLoader (updater)
# Author: Stephan Bärwolf
# Creation Date: 2012-09-01
# Tabsize: 4
# License: GNU GPL v2 (see License.txt)

F_CPU = 16000000
DEVICE = atmega8

# BOOTLOADER_ADDRESS is 1800 for 8k devices, 3800 for 16k and 7800 for 32k.
BOOTLOADER_ADDRESS = 0x1800
NEW_BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS)

# where the updating firmware should be located (starting address)
FLASHADDRESS = 0x0000


# PROGRAMMER contains AVRDUDE options to address your programmer
# PROGRAMMER = -c pony-stk200
PROGRAMMER = -c usbasp

#  since USBaspLoader supports HAVE_BLB11_SOFTW_LOCKBIT...
LOCKOPT = -U lock:w:0x3f:m

# standard atmega8 needs BODLEVEL to be programed, since it is a 5V device
# you may also want to UNprogram  SUT1 to get a SLOWER bootup (lfuse then would be 0x3f)
FUSEOPT_8 = -U hfuse:w:0xc0:m -U lfuse:w:0x1f:m

#untested (WARNING: may destroy ISP ability!)
#device always should select maximum bootloader-section size
#please mail tests-reports to matrixstorm@gmx.de:
FUSEOPT_32   = unknown
FUSEOPT_88   = -U hfuse:w:0xd6:m -U lfuse:w:0xdf:m -U efuse:w:0x00:m
FUSEOPT_164  = unknown
FUSEOPT_168  = -U hfuse:w:0xd6:m -U lfuse:w:0xdf:m -U efuse:w:0x00:m
FUSEOPT_324  = unknown
FUSEOPT_328  = -U lfuse:w:0xf7:m -U hfuse:w:0xda:m -U efuse:w:0x03:m
FUSEOPT_644  = unknown
FUSEOPT_128  = unknown
FUSEOPT_1284 = unknown

ifeq ($(DEVICE), atmega8)
  FUSEOPT = $(FUSEOPT_8)
else ifeq ($(DEVICE), atmega32)
  FUSEOPT = $(FUSEOPT_32)
else ifeq ($(DEVICE), atmega88)
  FUSEOPT = $(FUSEOPT_88)
else ifeq ($(DEVICE), atmega88a)
  FUSEOPT = $(FUSEOPT_88)
else ifeq ($(DEVICE), atmega88p)
  FUSEOPT = $(FUSEOPT_88)
else ifeq ($(DEVICE), atmega88pa)
  FUSEOPT = $(FUSEOPT_88)
else ifeq ($(DEVICE), atmega164a)
  FUSEOPT = $(FUSEOPT_164)
else ifeq ($(DEVICE), atmega164p)
  FUSEOPT = $(FUSEOPT_164)
else ifeq ($(DEVICE), atmega164pa)
  FUSEOPT = $(FUSEOPT_164)
else ifeq ($(DEVICE), atmega168)
  FUSEOPT = $(FUSEOPT_168)
else ifeq ($(DEVICE), atmega168a)
  FUSEOPT = $(FUSEOPT_168)
else ifeq ($(DEVICE), atmega168p)
  FUSEOPT = $(FUSEOPT_168)
else ifeq ($(DEVICE), atmega168pa)
  FUSEOPT = $(FUSEOPT_168)
else ifeq ($(DEVICE), atmega324a)
  FUSEOPT = $(FUSEOPT_324)
else ifeq ($(DEVICE), atmega324p)
  FUSEOPT = $(FUSEOPT_324)
else ifeq ($(DEVICE), atmega324pa)
  FUSEOPT = $(FUSEOPT_324)
else ifeq ($(DEVICE), atmega328)
  FUSEOPT = $(FUSEOPT_328)
else ifeq ($(DEVICE), atmega328p)
  FUSEOPT = $(FUSEOPT_328)
else ifeq ($(DEVICE), atmega644)
  FUSEOPT = $(FUSEOPT_644)
else ifeq ($(DEVICE), atmega644a)
  FUSEOPT = $(FUSEOPT_644)
else ifeq ($(DEVICE), atmega644p)
  FUSEOPT = $(FUSEOPT_644)
else ifeq ($(DEVICE), atmega644pa)
  FUSEOPT = $(FUSEOPT_644)
else ifeq ($(DEVICE), atmega128)
  FUSEOPT = $(FUSEOPT_128)
else ifeq ($(DEVICE), atmega1284)
  FUSEOPT = $(FUSEOPT_1284)
else ifeq ($(DEVICE), atmega1284p)
  FUSEOPT = $(FUSEOPT_1284)
else
  FUSEOPT = unknown
endif


# Tools:
AVRPATH = $(AVR8TOOLCHAINBINDIR)
AVRDUDE = @echo avrdude $(PROGRAMMER) -p $(DEVICE)

ECHO=@echo
GCC=@gcc
MAKE=@make
RM=@rm -f

CC=@$(AVRPATH)avr-gcc
OBC=@$(AVRPATH)avr-objcopy
OBD=@$(AVRPATH)avr-objdump
SIZ=@$(AVRPATH)avr-size

