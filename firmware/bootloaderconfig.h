/* Name: bootloaderconfig.h
 * Project: USBaspLoader
 * Author: Christian Starkjohann
 * Creation Date: 2007-12-08
 * Tabsize: 4
 * Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt)
 * This Revision: $Id: bootloaderconfig.h 729 2009-03-20 09:03:58Z cs $
 */

#ifndef __bootloaderconfig_h_included__
#define __bootloaderconfig_h_included__

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

/* ---------------------------- Hardware Config ---------------------------- */

#define USB_CFG_IOPORTNAME      D
/* This is the port where the USB bus is connected. When you configure it to
 * "B", the registers PORTB, PINB and DDRB will be used.
 */
#define JUMPER_BIT		7	/* old value was 0 */
/* 
 * jumper is connected to this bit in port D, active low
 */
#define USB_CFG_DMINUS_BIT      6	/* old value was 4 */
/* This is the bit number in USB_CFG_IOPORT where the USB D- line is connected.
 * This may be any bit in the port.
 */
#define USB_CFG_DPLUS_BIT       2
/* This is the bit number in USB_CFG_IOPORT where the USB D+ line is connected.
 * This may be any bit in the port. Please note that D+ must also be connected
 * to interrupt pin INT0!
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

#define HAVE_READ_LOCK_FUSE	    1
/*
 * enable the loaders capability to load its lfuse, hfuse and lockbits
 * ...However, programming of these is prohibited...
 */

#define HAVE_BLB11_SOFTW_LOCKBIT    1
/*
 * The IC itself do not need to prgra BLB11, but the bootloader will avaoid 
 * to erase itself from the bootregion
 */

#define HAVE_DOSPM_TUNNELCMD	    1
/*
 * When enabled, "HAVE_DOSPM_TUNNELCMD" will implement an PROGMEM ARRAY 
 * with up to 15 opcodes within BLS.
 * This array will be called "bootloader__do_spm", and implements the 
 * "do_spm" subroutine from atmels "Instruction Set Manual" Rev.0856I, page 140.
 * 
do_spm:
;input:	spmcrval determines SPM action
;disable interrupts if enabled, store status
;temp1 will be register:	r7
;temp2 will be register:	r8
;spmcrval will be register:	r9

in	temp2, SREG	; --> has to be done before calling
cli			; --> has to be done before calling
			;check for previous SPM complete
wait:
in	temp1, SPMCR
sbrc	temp1, SPMEN
rjmp	wait
			;SPM timed sequence
out	SPMCR, spmcrval
spm
			;restore SREG (to enable interrupts if originally enabled)
out	SREG, temp2
ret
 * 
 */

#define HAVE_EEPROM_PAGED_ACCESS    1
/* If HAVE_EEPROM_PAGED_ACCESS is defined to 1, page mode access to EEPROM is
 * compiled in. Whether page mode or byte mode access is used by AVRDUDE
 * depends on the target device. Page mode is only used if the device supports
 * it, e.g. for the ATMega88, 168 etc. You can save quite a bit of memory by
 * disabling page mode EEPROM access. Costs ~ 138 bytes.
 */
#define HAVE_EEPROM_BYTE_ACCESS     1
/* If HAVE_EEPROM_BYTE_ACCESS is defined to 1, byte mode access to EEPROM is
 * compiled in. Byte mode is only used if the device (as identified by its
 * signature) does not support page mode for EEPROM. It is required for
 * accessing the EEPROM on the ATMega8. Costs ~54 bytes.
 */
#define BOOTLOADER_CAN_EXIT         0
/* If this macro is defined to 1, the boot loader will exit shortly after the
 * programmer closes the connection to the device. Costs ~36 bytes.
 */
#define HAVE_CHIP_ERASE             0
/* If this macro is defined to 1, the boot loader implements the Chip Erase
 * ISP command. Otherwise pages are erased on demand before they are written.
 */
//#define SIGNATURE_BYTES             0x1e, 0x93, 0x07, 0     /* ATMega8 */
/* This macro defines the signature bytes returned by the emulated USBasp to
 * the programmer software. They should match the actual device at least in
 * memory size and features. If you don't define this, values for ATMega8,
 * ATMega88, ATMega168 and ATMega328 are guessed correctly.
 */

/* The following block guesses feature options so that the resulting code
 * should fit into 2k bytes boot block with the given device and clock rate.
 * Activate by passing "-DUSE_AUTOCONFIG=1" to the compiler.
 * This requires gcc 3.4.6 for small enough code size!
 */
#if USE_AUTOCONFIG
#   undef HAVE_EEPROM_PAGED_ACCESS
#   define HAVE_EEPROM_PAGED_ACCESS     (USB_CFG_CLOCK_KHZ >= 16000)
#   undef HAVE_EEPROM_BYTE_ACCESS
#   define HAVE_EEPROM_BYTE_ACCESS      1
#   undef BOOTLOADER_CAN_EXIT
#   define BOOTLOADER_CAN_EXIT          1
#   undef SIGNATURE_BYTES
#endif /* USE_AUTOCONFIG */

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

static inline void  bootLoaderInit(void)
{
    PORTD |= (1 << JUMPER_BIT);     /* activate pull-up */
//     deactivated by Stephan - reset after each avrdude op is annoing!
//     if(!(MCUCSR & (1 << EXTRF)))    /* If this was not an external reset, ignore */
//         leaveBootloader();
    MCUCSR = 0;                     /* clear all reset flags for next time */
}

static inline void  bootLoaderExit(void)
{
    PORTD = 0;                      /* undo bootLoaderInit() changes */
}

#define bootLoaderCondition()		((PIND & (1 << JUMPER_BIT)) == 0)

#endif /* __ASSEMBLER__ */

/* ------------------------------------------------------------------------- */

#endif /* __bootloader_h_included__ */
