#include "paralax.h"
#include <cbm.h>
#include <stdio.h>
#include <string.h>


#define SUN_EMPTYCHAR1 0
#define SUN_EMPTYCHAR2 0xfb
#define MNTAIN_EMPTYCHAR 0xbf
#define SUN_BGCOLOR (0x6+8)
#define BORDER_COLOR 0

extern unsigned char _SCREEN1_LOAD__[1000];
extern unsigned char _SCREEN2_LOAD__[1000];
extern unsigned char _COLORRAM_START__[1000];
extern unsigned char vicbankbits;
extern char ghostbyte;/*Must be external, else it would be impossible
  											to declare a variable at a specific adress*/
  											
/*externals outside function, to avoid the reservation of memmory in the
  BSS-Segment*/
  											
void init_parscreen(void){
  memset(&_SCREEN1_LOAD__, MNTAIN_EMPTYCHAR, sizeof(_SCREEN1_LOAD__));
  memset(&_SCREEN1_LOAD__, SUN_EMPTYCHAR1, (SUNAREAH-1)*SUNAREAW);
  memset(&_SCREEN2_LOAD__, SUN_EMPTYCHAR2, sizeof(_SCREEN2_LOAD__));
  memset(&_COLORRAM_START__, SUN_BGCOLOR, sizeof(_COLORRAM_START__));
  InitMountains();
  
  VIC.bordercolor = BORDER_COLOR;
  CIA2.pra = 0xc0|((unsigned char)&vicbankbits);
	
	#if DEBUG == 0
  	ghostbyte = 0xff;
  #else
  	ghostbyte = 0xf0;
  #endif
}