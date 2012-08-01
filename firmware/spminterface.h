/* Name: spminterface.h
 * Project: USBaspLoader
 * Author: Stephan Baerwolf
 * Creation Date: 2012-08-01
 * Copyright: (c) 2012 by Stephan Baerwolf
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef SPMINTERFACE_H_f70ba6adf7624275947e859bdbff0599
#define SPMINTERFACE_H_f70ba6adf7624275947e859bdbff0599

/*
 * spminterface.h offers a lightweight interface by inserting
 * an small machine-subroutine into the bootsection. (right 
 * after the interrupt-vector-table)
 * This subroutine can be called by normal code in order to 
 * enable it to program the flash (and depending on BLB11-lockbit
 * also the bootsection itself). Since SPM-calls from RWW-sections
 * will fail to work. The Routine will be called "bootloader__do_spm".
 * Its principle assembler-code is depicted below (real code is a
 * little machinedependen).
 * Interfaces will be the 8-bit registers r10..r13, for details 
 * also see below. As also the pageaddress-registes (Z and rampZ)
 * are interfaced via different registers, it is possible to call
 * this routine via indirect call (icall).
 * Traditionally it is also possible to rcall, therefore you can
 * define "bootloader__do_spm" for your normal code via defsym at 
 * linking time.
 * Example for an atmega8: "-Wl,--defsym=transfer_point=0x1826"
 * (since BOOTLOADER_ADDRESS is 0x1800 and there are 
 * 2x19 = 38 = 0x26 byte for interrupts)
 * 

bootloader__do_spm:
;disable interrupts (if enabled) before calling!
;you may also want to disable wdt, since this routine may busy-loop
;==================================================================
;-->INPUT:
;spmcr (spmcrval determines SPM action) will be register:	r10
;MCU dependend RA(MPZ should be transfered within register:	r11
;lo8(Z) should be transfered within register:			r12
;hi8(Z) should be transfered within register:			r13

;<-->USED/CHANGED:
;temp0 will be register:					r11
;temp1 will be register:					r12
;temp2 will be register:					r13
;Z (r31:r30) wil be changed during operation

;<--OUT:
;==================================================================

;load pageaddress (Z) from r13:12 since it may was used for icall
mov	rampZ,	r11
mov	r30,	r12
mov	r31,	r13

wait:			;check for previous SPM complete
in	temp1, SPMCR
sbrc	temp1, SPMEN
rjmp	wait

out	SPMCR, spmcrval	;SPM timed sequence
spm

ret

*
*/ 

#include <avr/io.h>
#include <avr/pgmspace.h>

#ifndef BOOTLOADER_ADDRESS
// this header is the interface for user-code




#else /*ifndef BOOTLOADER_ADDRESS*/
// this header is used directly within bootloader_do_spm
#include "bootloaderconfig.h"

#if HAVE_SPMINTEREFACE


/*
 * insert architecture dependend "bootloader_do_spm"-code
 * 
 * try to make this array as big as possible
 * (so bootloader always uses 2kbytes flash)
 */
#if defined (__AVR_ATmega8__) || defined (__AVR_ATmega8HVA__)
//assume  SPMCR==0x37, SPMEN==0x00
const uint16_t bootloader__do_spm[30] PROGMEM = {0x0000, 0x2dec, 0x2dfd, 0xb6c7, 0xfcc0, 0xcffd, 0xbea7, 0x95e8, 0x9508,
						 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
						 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
						 0xFFFF, 0xFFFF, 0xFFFF};
#else
  #error "bootloader__do_spm has to be adapted, since there is no architecture code, yet"
#endif  


#endif
#endif /*ifdef BOOTLOADER_ADDRESS*/

#endif
