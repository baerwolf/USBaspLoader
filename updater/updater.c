#ifndef F_CPU
  #define F_CPU 1000000UL /* 1 Mhz-Takt; hier richtigen Wert eintragen */
#endif

// 1) Test the "changed"     - feature by debugging a LED-PIN to high (PB0?)
// 2) Test the "needs_erase" - feature by debugging a LED-PIN to high (PB1?)
// #define mypgmdebug 1

#include "../firmware/spminterface.h"
#include "usbasploader.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include <avr/pgmspace.h>

#include <stdint.h>

#include <util/delay.h>
#include <string.h>

#include "../firmware/bootloaderconfig.h"
#if !HAVE_SPMINTEREFACE
  #error "bootloader does not support updating itself! (HAVE_SPMINTEREFACE)"
#endif

// helpful definitions and makros ////

#ifndef NEW_BOOTLOADER_ADDRESS
  #error "where should the new bootloader be positioned?"
#endif



#if (NEW_BOOTLOADER_ADDRESS != (funcaddr___bootloader__do_spm-(funcaddr___bootloader__do_spm % SPM_PAGESIZE)))
  #warning the new bootloader seems to be located elsewhere, than the current one!
#endif

#ifndef NEW_SPM_ADDRESS
  #warning I do not know where new "bootloader__do_spm" is located - assuming "NEW_BOOTLOADER_ADDRESS+(funcaddr___bootloader__do_spm % SPM_PAGESIZE)"
  #define NEW_SPM_ADDRESS (NEW_BOOTLOADER_ADDRESS+(funcaddr___bootloader__do_spm % SPM_PAGESIZE))
#endif

// TEMP_SPM supports up to 4 pages for "bootloader__do_spm"...
#define TEMP_SPM_NUMPAGE 4
#define TEMP_SPM_BLKSIZE (TEMP_SPM_NUMPAGE*SPM_PAGESIZE)
#ifndef TEMP_SPM_PAGEADR
  #warning "TEMP_SPM_PAGEADR" is not defined explicitly - will choose END OF FLASH !
  #define TEMP_SPM_PAGEADR ((FLASHEND+1) - TEMP_SPM_BLKSIZE)
#endif
#define TEMP_SPM_ADDRESS ((TEMP_SPM_PAGEADR) + (funcaddr___bootloader__do_spm % SPM_PAGESIZE))


#if (NEW_SPM_ADDRESS == funcaddr___bootloader__do_spm)
  #define new_do_spm	do_spm
#else
void new_do_spm(const uint32_t flash_byteaddress, const uint8_t spmcrval, const uint16_t dataword) {
    __do_spm_Ex(flash_byteaddress, spmcrval, dataword, NEW_SPM_ADDRESS >> 1);
}
#endif

void temp_do_spm(const uint32_t flash_byteaddress, const uint8_t spmcrval, const uint16_t dataword) {
    __do_spm_Ex(flash_byteaddress, spmcrval, dataword, TEMP_SPM_ADDRESS >> 1);
}



// some important consistency checks ////

//check if "NEW_BOOTLOADER_ADDRESS" is page-aligned
#if (NEW_BOOTLOADER_ADDRESS % SPM_PAGESIZE != 0) 
  #error "NEW_BOOTLOADER_ADDRESS" is not aligned to pages!
#endif

//check, if NEW_SPM_ADDRESS is an even address
#if ((NEW_SPM_ADDRESS % 2) != 0)
  #error NEW_SPM_ADDRESS must be an even address, since it contains executable code!
#endif



//check, if TEMP_SPM somehow overlaps with old SPM
#if (((TEMP_SPM_ADDRESS + TEMP_SPM_BLKSIZE + SPM_PAGESIZE) >= funcaddr___bootloader__do_spm) && (TEMP_SPM_ADDRESS <= (funcaddr___bootloader__do_spm + TEMP_SPM_BLKSIZE + SPM_PAGESIZE)))
  #error TEMP_SPM_ADDRESS overlaps "funcaddr___bootloader__do_spm"!
#endif

//check, if TEMP_SPM somehow overlaps with new SPM
#if (((TEMP_SPM_ADDRESS + TEMP_SPM_BLKSIZE + SPM_PAGESIZE) >= NEW_SPM_ADDRESS) && (TEMP_SPM_ADDRESS <= (NEW_SPM_ADDRESS + TEMP_SPM_BLKSIZE + SPM_PAGESIZE)))
  #error TEMP_SPM_ADDRESS overlaps "NEW_SPM_ADDRESS"!
#endif

//check, if TEMP_SPM_ADDRESS is an even address
#if ((TEMP_SPM_ADDRESS % 2) != 0)
  #error TEMP_SPM_ADDRESS must be an even address, since it contains executable code!
#endif

//check, if TEMP_SPM_ADDRESS fits into flash
#if ((TEMP_SPM_PAGEADR + TEMP_SPM_BLKSIZE) > (FLASHEND+1))
  #error TEMP_SPM_ADDRESS exceeds flashend!
#endif

//check if size too low
#if (SIZEOF_new_firmware <= (TEMP_SPM_BLKSIZE + (NEW_SPM_ADDRESS - NEW_BOOTLOADER_ADDRESS)))
  #error empty firmware!
#endif

//check if size too high
#if (SIZEOF_new_firmware > ((FLASHEND+1)-NEW_BOOTLOADER_ADDRESS))
  #error firmware too big! firmware does not fit into flash memory!
#endif

// main code ////

/*
 * in this case a near address
 */
typedef uint32_t mypgm_addr_t;
typedef void (*mypgm_spminterface)(const uint32_t flash_byteaddress, const uint8_t spmcrval, const uint16_t dataword);


#if 0
size_t mypgm_readpage(const mypgm_addr_t byteaddress,const void* buffer, const size_t bufferbytesize) {
  size_t	result		= (bufferbytesize < SPM_PAGESIZE)?bufferbytesize:SPM_PAGESIZE;
  size_t	pagesize	= result >> 1;
  uint16_t	*pagedata	= (void*)buffer;
  mypgm_addr_t	pageaddr	= byteaddress - (byteaddress % SPM_PAGESIZE);
  size_t	i;
  
  for (i=0;i<pagesize;i+=1) {
    pagedata[i]=pgm_read_word(pageaddr);
    pageaddr+=2;
  }
  
  return result;
}
#else
// replace it somehow with "memcpy_PF" in order to save some ops...
size_t mypgm_readpage(const mypgm_addr_t byteaddress,const void* buffer, const size_t bufferbytesize) {
  size_t	result		= (bufferbytesize < SPM_PAGESIZE)?bufferbytesize:SPM_PAGESIZE;
  mypgm_addr_t	pageaddr	= byteaddress - (byteaddress % SPM_PAGESIZE);
  
  memcpy_PF((void*)buffer, (uint_farptr_t)pageaddr, result);
  
  return result;
}
#endif

size_t mypgm_WRITEpage(const mypgm_addr_t byteaddress,const void* buffer, const size_t bufferbytesize, mypgm_spminterface spmfunc) {
  size_t	result		= (bufferbytesize < SPM_PAGESIZE)?bufferbytesize:SPM_PAGESIZE;
  size_t	pagesize	= result >> 1;
  uint16_t	*pagedata	= (void*)buffer;
  mypgm_addr_t	pageaddr_bakup	= byteaddress - (byteaddress % SPM_PAGESIZE);
  mypgm_addr_t	pageaddr	= pageaddr_bakup;
  
  uint8_t	changed=0, needs_erase=0;
  uint16_t	deeword;
  size_t	i;
  
  // just check, if page needs a rewrite or an erase...
  for (i=0;i<pagesize;i+=1) {
    deeword=pgm_read_word(pageaddr);

    if (deeword != pagedata[i]) changed=1;

    /*
     *  deeword = x
     *  buffer  = y
     * 
     *  1 ? 1 ==> 1
     *  1 ? 0 ==> 1
     *  0 ? 1 ==> 0
     *  0 ? 0 ==> 1
     * 
     * ==> /(/x * y) ==> x + /y
     */
    deeword |= ~pagedata[i];
    if ((~deeword) != 0) needs_erase=1;
      
    pageaddr+=2;
  }

  if (changed) {
#ifdef mypgmdebug
    DDRB  |= (1<<PB0);
    PORTB |= (1<<PB0);
#endif
    
    if (needs_erase) {
      //do a page-erase, ATTANTION: flash only can be erased a limited number of times !
      spmfunc(pageaddr_bakup, 0x3, 0);
#ifndef mypgmdebug
    }
#else 
    } else {
      DDRB  |= (1<<PB1);
      PORTB |= (1<<PB1);
    }
#endif

    
    // from now on, the page is assumed empty !! (hopefully our code is located somewhere else!)
    // now, fill the tempoary buffer
    // ATTANTION: see comment on "do_spm" !
    pageaddr	= pageaddr_bakup;
    for (i=0;i<pagesize;i+=1) {
      spmfunc(pageaddr, 0x1, pagedata[i]);
      pageaddr+=2;
    }
    
    // so, now finally write the page to the FLASH
    spmfunc(pageaddr_bakup, 0x5, 0);
  } else {
    // no change - no write...
    result = 0;
  }
  
  
  return result;
}

// #pragma GCC diagnostic ignored "-Wno-pointer-to-int-cast"
int main(void)
{
    size_t  i;
    uint8_t buffer[SPM_PAGESIZE];
    
    wdt_disable();
    cli();

#ifdef mypgmdebug
    DDRD  |= (1<<PD3);
    DDRD  |= (1<<PD5);
    PORTD |= (1<<PD3);
    PORTD |= (1<<PD5);
#endif



    // check if firmware would change...
    buffer[0]=0;
    for (i=0;i<SIZEOF_new_firmware;i+=2) {
      uint16_t a, b;
      a=pgm_read_word((void*)&new_firmware[i]);
      b=pgm_read_word(NEW_BOOTLOADER_ADDRESS+i);
      if (a!=b) {
	buffer[0]=1;
	break;
      }
    }



    // need to change the firmware...
    if (buffer[0]) {


      // A
      // copy the current "bootloader__do_spm" to tempoary position via std. "bootloader__do_spm"
      for (i=0;i<TEMP_SPM_BLKSIZE;i+=SPM_PAGESIZE) {
	mypgm_WRITEpage(TEMP_SPM_PAGEADR+i, buffer, mypgm_readpage(funcaddr___bootloader__do_spm+i, buffer, sizeof(buffer)), do_spm);
  #ifdef mypgmdebug
	PORTD ^= (1<<PD3);
	_delay_ms(500);
	PORTD ^= (1<<PD3);
	_delay_ms(500);
  #endif
      }

      // B
      // start updating the firmware to "NEW_BOOTLOADER_ADDRESS" until at least "TEMP_SPM_BLKSIZE"-bytes after "NEW_SPM_ADDRESS" were written
      // therefore use the tempoary "bootloader__do_spm" (since we most probably will overwrite the default do_spm)
      for (i=0;;i+=SPM_PAGESIZE) {
	memset((void*)buffer, 0xff, sizeof(buffer));
	memcpy_PF((void*)buffer, (uint_farptr_t)((void*)&new_firmware[i]), ((SIZEOF_new_firmware-i)>sizeof(buffer))?sizeof(buffer):(SIZEOF_new_firmware-i));
	
	mypgm_WRITEpage(NEW_BOOTLOADER_ADDRESS+i, buffer, sizeof(buffer), temp_do_spm);
	
  #ifdef mypgmdebug
	PORTD ^= (1<<PD3);
	_delay_ms(500);
	PORTD ^= (1<<PD3);
	_delay_ms(500);
  #endif

	if ((NEW_BOOTLOADER_ADDRESS+i) > (NEW_SPM_ADDRESS+TEMP_SPM_BLKSIZE)) break;
      }

      // C
      // continue writeing the new_firmware after "NEW_SPM_ADDRESS+TEMP_SPM_BLKSIZE" this time use the "new_do_spm"
      for (;i<SIZEOF_new_firmware;i+=SPM_PAGESIZE) {
	memset((void*)buffer, 0xff, sizeof(buffer));
	memcpy_PF((void*)buffer, (uint_farptr_t)((void*)&new_firmware[i]), ((SIZEOF_new_firmware-i)>sizeof(buffer))?sizeof(buffer):(SIZEOF_new_firmware-i));

	mypgm_WRITEpage(NEW_BOOTLOADER_ADDRESS+i, buffer, sizeof(buffer), new_do_spm);
	
  #ifdef mypgmdebug
	PORTD ^= (1<<PD3);
	_delay_ms(500);
	PORTD ^= (1<<PD3);
	_delay_ms(500);
  #endif
      }



    }

#ifdef mypgmdebug
    PORTD |= (1<<PD3);
#endif
    while(1)
    {
#ifdef mypgmdebug
      PORTD ^= (1<<PD5);
      _delay_ms(100);
#endif
    }

}
