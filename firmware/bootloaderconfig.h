/* Name: bootloaderconfig.h
 * Project: USBaspLoader
 * Author: Christian Starkjohann
 * Author: Stephan Baerwolf
 * Creation Date: 2007-12-08
 * Modification Date: 2013-03-31
 * Tabsize: 4
 * Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef __bootloaderconfig_h_included__
#define __bootloaderconfig_h_included__
#include <avr/io.h>

/*
General Description:
This file (together with some settings in Makefile) configures the boot loader
according to the hardware.

This file contains (besides the hardware configuration normally found in
usbconfig.h) two functions or macros: bootLoaderInit() and
bootLoaderCondition(). Whether you implement them as macros or as static
inline functions is up to you, decide based on code size and convenience.

bootLoaderInit() is called as one of the first actions after reset. It should
be a minimum initialization of the hardware so that the boot loader condition
can be read. This will usually consist of activating a pull-up resistor for an
external jumper which selects boot loader mode.

bootLoaderCondition() is called immediately after initialization and in each
main loop iteration. If it returns TRUE, the boot loader will be active. If it
returns FALSE, the boot loader jumps to address 0 (the loaded application)
immediately.

For compatibility with Thomas Fischl's avrusbboot, we also support the macro
names BOOTLOADER_INIT and BOOTLOADER_CONDITION for this functionality. If
these macros are defined, the boot loader usees them.
*/

/* ---------------------------- Macro Magic ---------------------------- */
#define		PIN_CONCAT(a,b)			a ## b
#define		PIN_CONCAT3(a,b,c)		a ## b ## c

#define		PIN_PORT(a)			PIN_CONCAT(PORT, a)
#define		PIN_PIN(a)			PIN_CONCAT(PIN, a)
#define		PIN_DDR(a)			PIN_CONCAT(DDR, a)

#define		PIN(a, b)			PIN_CONCAT3(P, a, b)

/* ---------------------------- Hardware Config ---------------------------- */

#ifndef USB_CFG_IOPORTNAME
  #define USB_CFG_IOPORTNAME      D
#endif
/* This is the port where the USB bus is connected. When you configure it to
 * "B", the registers PORTB, PINB and DDRB will be used.
 */
#ifndef USB_CFG_INTPORT_BIT
  #if (defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__))
    #define USB_CFG_INTPORT_BIT 0
  #else
    #define USB_CFG_INTPORT_BIT 2
  #endif
#endif
/* Not all devices have their INT0 on PD2.
 * Since "INT0" and "USB_CFG_DPLUS_BIT" should get the same signals,
 * map them to be ideally the same:
 * So abstract "USB_CFG_DPLUS_BIT" to this one here.
 */

#ifndef USB_CFG_DMINUS_BIT
  /* This is Revision 3 and later (where PD6 and PD7 were swapped */
  #define USB_CFG_DMINUS_BIT      7    /* Rev.2 and previous was 6 */
#endif
/* This is the bit number in USB_CFG_IOPORT where the USB D- line is connected.
 * This may be any bit in the port.
 */
#ifndef USB_CFG_DPLUS_BIT
  #define USB_CFG_DPLUS_BIT       USB_CFG_INTPORT_BIT
#endif
/* This is the bit number in USB_CFG_IOPORT where the USB D+ line is connected.
 * This may be any bit in the port. Please note that D+ must also be connected
 * to interrupt pin INT0!
 */
#ifndef JUMPER_PORT
  #define JUMPER_PORT		USB_CFG_IOPORTNAME
#endif
/* 
 * jumper is connected to this port
 */
#ifndef JUMPER_BIT
  /* This is Revision 3 and later (where PD6 and PD7 were swapped */
  #define JUMPER_BIT           6       /* Rev.2 and previous was 7 */
#endif
/* 
 * jumper is connected to this bit in port "JUMPER_PORT", active low
 */

#define USB_CFG_CLOCK_KHZ       (F_CPU/1000)
/* Clock rate of the AVR in MHz. Legal values are 12000, 16000 or 16500.
 * The 16.5 MHz version of the code requires no crystal, it tolerates +/- 1%
 * deviation from the nominal frequency. All other rates require a precision
 * of 2000 ppm and thus a crystal!
 * Default if not specified: 12 MHz
 */

/* ----------------------- Optional Hardware Config ------------------------ */

/* #define USB_CFG_PULLUP_IOPORTNAME   D */
/* If you connect the 1.5k pullup resistor from D- to a port pin instead of
 * V+, you can connect and disconnect the device from firmware by calling
 * the macros usbDeviceConnect() and usbDeviceDisconnect() (see usbdrv.h).
 * This constant defines the port on which the pullup resistor is connected.
 */
/* #define USB_CFG_PULLUP_BIT          4 */
/* This constant defines the bit number in USB_CFG_PULLUP_IOPORT (defined
 * above) where the 1.5k pullup resistor is connected. See description
 * above for details.
 */

/* ------------------------------------------------------------------------- */
/* ---------------------- feature / code size options ---------------------- */
/* ------------------------------------------------------------------------- */

#ifndef CONFIG_NO__HAVE_READ_LOCK_FUSE
  #define HAVE_READ_LOCK_FUSE	    1
#else
  #define HAVE_READ_LOCK_FUSE	    0
#endif
/*
 * enable the loaders capability to load its lfuse, hfuse and lockbits
 * ...However, programming of these is prohibited...
 */

#ifndef CONFIG_NO__HAVE_BLB11_SOFTW_LOCKBIT
  #define HAVE_BLB11_SOFTW_LOCKBIT    1
#else
  #define HAVE_BLB11_SOFTW_LOCKBIT    0
#endif
/*
 * The IC itself do not need to prgra BLB11, but the bootloader will avaoid 
 * to erase itself from the bootregion
 */

#ifndef CONFIG_NO__HAVE_SPMINTEREFACE
  #define HAVE_SPMINTEREFACE	    1
#else
  #define HAVE_SPMINTEREFACE	    0
#endif
/*
 * Since code within normal section of application memory (rww-section) is
 * not able to call spm for programming flash-pages, this option (when
 * enabled) will insert a small subroutine into the bootloader-section
 * to enable applications to circumvent this limitation and make them
 * able to program the flash in a similar way as the bootloader does, too.
 * For further details see "spminterface.h", which implements this 
 * feature.
 */

#define HAVE_SPMINTEREFACE_NORETMAGIC	1
/*
 * If sth. went wrong within "bootloader__do_spm" and this macro is ACTIVATED,
 * then "bootloader__do_spm" will not return the call and loop infinity instead.
 * 
 * This feature prevents old updaters to do sth. undefined on wrong magic.
 */

/* all boards should use a magic to make it safe to confuse updatefiles :-)  */
#define HAVE_SPMINTEREFACE_MAGICVALUE    0
/* If this feature is enabled (value != 0), the configured 32bit value is 
 * used as a magic value within spminterface. "bootloader__do_spm" will check
 * additional four (4) registers for this value and only proceed, if they contain
 * the right value. With this feature you can identify your board and avoid
 * updating the wrong bootloader to the wrong board!
 * 
 * Not all values are possible - "SPMINTEREFACE_MAGICVALUE" must be very sparse!
 * To avoid collisions, magic-values will be organized centrally by Stephan
 * Following values are definitly blocked or reserved and must not be used:
 * 	0x00000000, 0x12345678,
 * 	0x00a500a5, 0x00a5a500, 0xa50000a5, 0xa500a500,
 * 	0x005a005a, 0x005a5a00, 0x5a00005a, 0x5a005a00,
 * 	0x5aa55aa5, 0x5aa5a55a, 0xa55a5aa5, 0xa55aa55a,
 * 	0x5a5a5a5a, 0xa5a5a5a5,
 * 	0xffa5ffa5, 0xffa5a5ff, 0xa5ffffa5, 0xa5ffa5ff,
 * 	0xff5aff5a, 0xff5a5aff, 0x5affff5a, 0x5aff5aff,
 * 	0x00ff00ff, 0x00ffff00, 0xff0000ff, 0xff00ff00,
 * 	0xffffffff
 * 
 * To request your own magic, please send at least following information
 * about yourself and your board together within an informal request to:
 * stephan@matrixstorm.com / matrixstorm@gmx.de / stephan.baerwolf@tu-ilmenau.de
 * 	  - your name
 * 	  - your e-mail
 * 	  - your project (maybe an url?)
 * 	  - your type of MCU used
 * 	--> your used "BOOTLOADER_ADDRESS" (since same magics can be reused for different "BOOTLOADER_ADDRESS")
 * 
 * There may be no garanty for it, but Stephan will then send you an
 * response with a "SPMINTEREFACE_MAGICVALUE" just for your board/project...
 * WITH REQUESTING A MAGIC YOU AGREE TO PUBLISHED YOUR DATA SEND WITHIN THE REQUEST 
 */

#ifndef CONFIG_NO__EEPROM_PAGED_ACCESS
#	define HAVE_EEPROM_PAGED_ACCESS    1
#else
#	define HAVE_EEPROM_PAGED_ACCESS    0
#endif
/* If HAVE_EEPROM_PAGED_ACCESS is defined to 1, page mode access to EEPROM is
 * compiled in. Whether page mode or byte mode access is used by AVRDUDE
 * depends on the target device. Page mode is only used if the device supports
 * it, e.g. for the ATMega88, 168 etc. You can save quite a bit of memory by
 * disabling page mode EEPROM access. Costs ~ 138 bytes.
 */

#ifndef CONFIG_NO__EEPROM_BYTE_ACCESS
#	define HAVE_EEPROM_BYTE_ACCESS     1
#else
#	define HAVE_EEPROM_BYTE_ACCESS     0
#endif
/* If HAVE_EEPROM_BYTE_ACCESS is defined to 1, byte mode access to EEPROM is
 * compiled in. Byte mode is only used if the device (as identified by its
 * signature) does not support page mode for EEPROM. It is required for
 * accessing the EEPROM on the ATMega8. Costs ~54 bytes.
 */

#ifndef CONFIG_NO__BOOTLOADER_CAN_EXIT
#	define BOOTLOADER_CAN_EXIT         1
#else
#	define BOOTLOADER_CAN_EXIT         0
#endif
/* If this macro is defined to 1, the boot loader will exit shortly after the
 * programmer closes the connection to the device. Costs extra bytes.
 */

#ifndef CONFIG_NO__CHIP_ERASE
#	define HAVE_CHIP_ERASE             1
#else
#	define HAVE_CHIP_ERASE             0
#endif
/* If this macro is defined to 1, the boot loader implements the Chip Erase
 * ISP command. Otherwise pages are erased on demand before they are written.
 */
#ifndef CONFIG_NO__ONDEMAND_PAGEERASE
#	define HAVE_ONDEMAND_PAGEERASE            1
#else
#	define HAVE_ONDEMAND_PAGEERASE            0
#endif
/* Even if "HAVE_CHIP_ERASE" is avtivated - enabling the "HAVE_ONDEMAND_PAGEERASE"-
 * feature the bootloader will erase pages on demand short before writing new data
 * to it.
 * If pages are not erase before reprogram (for example because user call avrdude -D)
 * then data may become inconsistent since writing only allow to unset bits in the flash.
 * This feature may prevent this...
 */

#ifndef CONFIG_NO__NEED_WATCHDOG
#	define NEED_WATCHDOG		1
#else
#	define NEED_WATCHDOG		0
#endif
/* ATTANTION: This macro MUST BE 1, if the MCU has reset enabled watchdog (WDTON is 0).
 * If this macro is defined to 1, the bootloader implements an additional "wdt_disable()"
 * after its contional entry point.
 * If the used MCU is fused not to enable watchdog after reset (WDTON is 1 - safty level 1)
 * then "NEED_WATCHDOG" may be deactivated in order to save some memory.
 */

#ifndef CONFIG_NO__PRECISESLEEP
#	define HAVE_UNPRECISEWAIT	0
#else
#	define HAVE_UNPRECISEWAIT	1
#endif
/* This macro enables hand-optimized assembler code
 * instead to use _sleep_ms for delaying USB enumeration.
 * Because normally these timings do not need to be exact,
 * the optimized assembler code does not need to be precise.
 * Therefore it is very small, which saves some PROGMEM bytes!
 */

#ifndef CONFIG_NO__FLASH_BYTE_READACCESS
#	define HAVE_FLASH_BYTE_READACCESS	1
#else
#	define HAVE_FLASH_BYTE_READACCESS	0
#endif
/* If HAVE_FLASH_BYTE_READACCESS is defined to 1, byte mode access to FLASH is
 * compiled in. Byte mode sometimes might be used by some programming softwares
 * (avrdude in terminal mode). Without this feature the device would return "0"
 * instead the right content of the flash memory.
 */

#ifdef CONFIG_USE__EXCESSIVE_ASSEMBLER
#	define USE_EXCESSIVE_ASSEMBLER		1
#else
#	define USE_EXCESSIVE_ASSEMBLER		0
#endif
/* This macro enables large codeareas of hand-optimized assembler code.
 * WARNING:
 * It will only work properly on devices with <64k of flash memory and SRAM.
 * Some configuration macros (when changed) may not be applied correctly
 * (since their behaviour is raced within asm)!
 * Nevertheless this feature saves lots of memory.
 */

#ifdef CONFIG_USE__BOOTUP_CLEARRAM
#	define USE_BOOTUP_CLEARRAM		1
#else
#	define USE_BOOTUP_CLEARRAM		0
#endif
/* This macro enables some (init3) code, executed at bootup.
 * This codefragment will safely overwrite the whole SRAM with "0"
 * (except registers and IO), since RESET will NOT clear old RAM content.
 */

//#define SIGNATURE_BYTES             0x1e, 0x93, 0x07, 0     /* ATMega8 */
/* This macro defines the signature bytes returned by the emulated USBasp to
 * the programmer software. They should match the actual device at least in
 * memory size and features. If you don't define this, values for ATMega8,
 * ATMega88, ATMega168 and ATMega328 are guessed correctly.
 */


/* ------------------------------------------------------------------------- */

/* Example configuration: Port D bit 3 is connected to a jumper which ties
 * this pin to GND if the boot loader is requested. Initialization allows
 * several clock cycles for the input voltage to stabilize before
 * bootLoaderCondition() samples the value.
 * We use a function for bootLoaderInit() for convenience and a macro for
 * bootLoaderCondition() for efficiency.
 */

#ifndef __ASSEMBLER__   /* assembler cannot parse function definitions */

#ifndef MCUCSR          /* compatibility between ATMega8 and ATMega88 */
#   define MCUCSR   MCUSR
#endif

/* WARNING:
 * following commands and macros may not be evaluated properly when 'USE_EXCESSIVE_ASSEMBLER"
 */

static inline void  bootLoaderInit(void)
{
    PIN_DDR(JUMPER_PORT)  = 0;
    PIN_PORT(JUMPER_PORT) = (1<< PIN(JUMPER_PORT, JUMPER_BIT)); /* activate pull-up */

//     deactivated by Stephan - reset after each avrdude op is annoing!
//     if(!(MCUCSR & (1 << EXTRF)))    /* If this was not an external reset, ignore */
//         leaveBootloader();

    MCUCSR = 0;                     /* clear all reset flags for next time */
}

static inline void  bootLoaderExit(void)
{
    PIN_PORT(JUMPER_PORT) = 0;		/* undo bootLoaderInit() changes */
}

#define bootLoaderCondition()		((PIN_PIN(JUMPER_PORT) & (1 << PIN(JUMPER_PORT, JUMPER_BIT))) == 0)

#endif /* __ASSEMBLER__ */

/* ------------------------------------------------------------------------- */

#endif /* __bootloader_h_included__ */
