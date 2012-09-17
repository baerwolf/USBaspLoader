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
FUSEOPT = $(FUSEOPT_8)

# standard atmega8 needs BODLEVEL to be programed, since it is a 5V device
# you may also want to UNprogram  SUT1 to get a SLOWER bootup (lfuse then would be 0x3f)
FUSEOPT_8 = -U hfuse:w:0xc0:m -U lfuse:w:0x1f:m

FUSEOPT_88 = -U hfuse:w:0xd6:m -U lfuse:w:0xdf:m -U efuse:w:0x00:m
FUSEOPT_168 = -U hfuse:w:0xd6:m -U lfuse:w:0xdf:m -U efuse:w:0x00:m
FUSEOPT_328 = -U lfuse:w:0xf7:m -U hfuse:w:0xda:m -U efuse:w:0x03:m
# You may have to change the order of these -U commands.

#---------------------------------------------------------------------
# ATMega8
#---------------------------------------------------------------------
# Fuse high byte:
# 0xc0 = 1 1 0 0   0 0 0 0 <-- BOOTRST (boot reset vector at 0x1800)
#        ^ ^ ^ ^   ^ ^ ^------ BOOTSZ0
#        | | | |   | +-------- BOOTSZ1
#        | | | |   + --------- EESAVE (preserve EEPROM over chip erase)
#        | | | +-------------- CKOPT (full output swing)
#        | | +---------------- SPIEN (allow serial programming)
#        | +------------------ WDTON (WDT not always on)
#        +-------------------- RSTDISBL (reset pin is enabled)
# Fuse low byte:
# 0x1f = 0 0 0 1   1 1 1 1
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (external >8M crystal)
#        | |  +--------------- SUT 1..0 (crystal osc, BOD enabled)
#        | +------------------ BODEN (BrownOut Detector enabled)
#        +-------------------- BODLEVEL (4.5V)
#---------------------------------------------------------------------
# ATMega88, ATMega168
#---------------------------------------------------------------------
# Fuse extended byte:
# 0x00 = 0 0 0 0   0 0 0 0 <-- BOOTRST (boot reset vector at 0x1800)
#                    \+/
#                     +------- BOOTSZ (00 = 2k bytes)
# Fuse high byte:
# 0xd6 = 1 1 0 1   0 1 1 0
#        ^ ^ ^ ^   ^ \-+-/
#        | | | |   |   +------ BODLEVEL 0..2 (110 = 1.8 V)
#        | | | |   + --------- EESAVE (preserve EEPROM over chip erase)
#        | | | +-------------- WDTON (if 0: watchdog always on)
#        | | +---------------- SPIEN (allow serial programming)
#        | +------------------ DWEN (debug wire enable)
#        +-------------------- RSTDISBL (reset pin is enabled)
# Fuse low byte:
# 0xdf = 1 1 0 1   1 1 1 1
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (external >8M crystal)
#        | |  +--------------- SUT 1..0 (crystal osc, BOD enabled)
#        | +------------------ CKOUT (if 0: Clock output enabled)
#        +-------------------- CKDIV8 (if 0: divide by 8)
#---------------------------------------------------------------------
# ATMega328P
#---------------------------------------------------------------------
# Fuse extended byte:
# 0x03 = - - - -   - 0 1 1
#                    \-+-/
#                      +------ BODLEVEL 0..2 (011 = 4.3V)
# Fuse high byte:
# 0xda = 1 1 0 1   1 0 1 0 <-- BOOTRST (0 = jump to bootloader at start)
#        ^ ^ ^ ^   ^ \+/
#        | | | |   |  +------- BOOTSZ 0..1 (01 = 2KB starting at 0x7800)
#        | | | |   + --------- EESAVE (don't preserve EEPROM over chip erase)
#        | | | +-------------- WDTON (1 = watchdog disabled at start)
#        | | +---------------- SPIEN (0 = allow serial programming)
#        | +------------------ DWEN (1 = debug wire disable)
#        +-------------------- RSTDISBL (1 = reset pin is enabled)
# Fuse low byte:
# 0xf7 = 1 1 1 1   0 1 1 1
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (0111 = external full-swing crystal)
#        | |  +--------------- SUT 1..0 (11 = startup time 16K CK/14K + 65ms)
#        | +------------------ CKOUT (1 = clock output disabled)
#        +-------------------- CKDIV8 (1 = do not divide clock by 8)


###############################################################################

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

