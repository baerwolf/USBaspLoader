# Name: Makefile.inc
# Project: USBaspLoader (updater)
# Author: Stephan Bärwolf
# Creation Date: 2012-09-01
# Tabsize: 4
# License: GNU GPL v2 (see License.txt)

F_CPU = 16000000
DEVICE = atmega8

# where the updating firmware should be located (starting address)
FLASHADDRESS = 0x0000

# some MCUs with small BLS (bootloader section) need to deactivate some
# features in order to fit into BLS.
# If this option is enabled, also essential features may become deactivated
# Esp. you MUST ensure not to have watchdog initially enabled when using this!
DANGEROUS=0

# PROGRAMMER contains AVRDUDE options to address your programmer
# PROGRAMMER = -c pony-stk200
PROGRAMMER = -c usbasp

#  since USBaspLoader supports HAVE_BLB11_SOFTW_LOCKBIT...
LOCKOPT = -U lock:w:0x3f:m

# standard atmega8 needs BODLEVEL to be programed, since it is a 5V device
# you may also want to UNprogram  SUT1 to get a SLOWER bootup (lfuse then would be 0x3f)
FUSEOPT_8            = -U hfuse:w:0xc0:m -U lfuse:w:0x1f:m
BOOTLOADER_ADDRESS_8 = 0x1800


#untested (WARNING: may destroy ISP ability!)
#device always should select maximum bootloader-section size
#please mail tests-reports to matrixstorm@gmx.de:
FUSEOPT_32              = unknown
BOOTLOADER_ADDRESS_32   = 0x7000

FUSEOPT_88              = -U hfuse:w:0xd6:m -U lfuse:w:0xdf:m -U efuse:w:0x00:m
BOOTLOADER_ADDRESS_88   = 0x1800

FUSEOPT_164             = unknown
BOOTLOADER_ADDRESS_164  = 0x3800

FUSEOPT_168             = -U hfuse:w:0xd6:m -U lfuse:w:0xdf:m -U efuse:w:0x00:m
BOOTLOADER_ADDRESS_168  = 0x3800

FUSEOPT_324             = unknown
BOOTLOADER_ADDRESS_324  = 0x7000

FUSEOPT_328             = -U lfuse:w:0xf7:m -U hfuse:w:0xda:m -U efuse:w:0x03:m
BOOTLOADER_ADDRESS_328  = 0x7000

FUSEOPT_644             = unknown
BOOTLOADER_ADDRESS_644  = 0xE000

FUSEOPT_128             = unknown
BOOTLOADER_ADDRESS_128  = 0x1E000

FUSEOPT_1284            = unknown
BOOTLOADER_ADDRESS_1284 = 0x1E000



ifeq ($(DANGEROUS), 1)
  CHOOSEFLASHSAVE = -DCONFIG_NO__NEED_WATCHDOG
else
  CHOOSEFLASHSAVE = -DCONFIG_NO__BOOTLOADER_CAN_EXIT
endif


#autoselect logic
DEFINES = #-DDEBUG_LEVEL=2
ifeq ($(DEVICE), atmega8)
  FUSEOPT            = $(FUSEOPT_8)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_8)  
else ifeq ($(DEVICE), atmega32)
  FUSEOPT            = $(FUSEOPT_32)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_32)
else ifeq ($(DEVICE), atmega88)
  FUSEOPT            = $(FUSEOPT_88)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_88)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE
else ifeq ($(DEVICE), atmega88a)
  FUSEOPT            = $(FUSEOPT_88)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_88)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE
else ifeq ($(DEVICE), atmega88p)
  FUSEOPT            = $(FUSEOPT_88)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_88)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE
else ifeq ($(DEVICE), atmega88pa)
  FUSEOPT            = $(FUSEOPT_88)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_88)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE
else ifeq ($(DEVICE), atmega164a)
  FUSEOPT            = $(FUSEOPT_164)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_164)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE -DCONFIG_NO__BOOTLOADER_CAN_EXIT
else ifeq ($(DEVICE), atmega164p)
  FUSEOPT            = $(FUSEOPT_164)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_164)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE -DCONFIG_NO__BOOTLOADER_CAN_EXIT
else ifeq ($(DEVICE), atmega164pa)
  FUSEOPT            = $(FUSEOPT_164)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_164)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE -DCONFIG_NO__BOOTLOADER_CAN_EXIT
else ifeq ($(DEVICE), atmega168)
  FUSEOPT            = $(FUSEOPT_168)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_168)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE $(CHOOSEFLASHSAVE)
else ifeq ($(DEVICE), atmega168a)
  FUSEOPT            = $(FUSEOPT_168)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_168)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE $(CHOOSEFLASHSAVE)
else ifeq ($(DEVICE), atmega168p)
  FUSEOPT            = $(FUSEOPT_168)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_168)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE $(CHOOSEFLASHSAVE)
else ifeq ($(DEVICE), atmega168pa)
  FUSEOPT            = $(FUSEOPT_168)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_168)
  DEFINES	     += -DCONFIG_NO__HAVE_READ_LOCK_FUSE $(CHOOSEFLASHSAVE)
else ifeq ($(DEVICE), atmega324a)
  FUSEOPT            = $(FUSEOPT_324)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_324)
else ifeq ($(DEVICE), atmega324p)
  FUSEOPT            = $(FUSEOPT_324)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_324)
else ifeq ($(DEVICE), atmega324pa)
  FUSEOPT            = $(FUSEOPT_324)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_324)
else ifeq ($(DEVICE), atmega328)
  FUSEOPT            = $(FUSEOPT_328)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_328)
else ifeq ($(DEVICE), atmega328p)
  FUSEOPT            = $(FUSEOPT_328)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_328)
else ifeq ($(DEVICE), atmega644)
  FUSEOPT            = $(FUSEOPT_644)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_644)
else ifeq ($(DEVICE), atmega644a)
  FUSEOPT            = $(FUSEOPT_644)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_644)
else ifeq ($(DEVICE), atmega644p)
  FUSEOPT            = $(FUSEOPT_644)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_644)
else ifeq ($(DEVICE), atmega644pa)
  FUSEOPT            = $(FUSEOPT_644)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_644)
else ifeq ($(DEVICE), atmega128)
  FUSEOPT            = $(FUSEOPT_128)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_128)
else ifeq ($(DEVICE), atmega1284)
  FUSEOPT            = $(FUSEOPT_1284)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_1284)
else ifeq ($(DEVICE), atmega1284p)
  FUSEOPT            = $(FUSEOPT_1284)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_1284)
else
  FUSEOPT = unknown
endif

NEW_BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS)

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

