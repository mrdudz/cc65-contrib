#include "paralax.h"
#include <cbm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <conio.h>
#include <stdbool.h>
#include <time.h>

#define SUNWINW 40
#define SUNWINH 19
#define CPU_PORT (*(unsigned char*) 1)
#define FRAME_CYCLES 18600
#define WAITCYCLES (6*FRAME_CYCLES/2)

#define FADECODE 0
#define WANKCODE 1
//constants for createspeedcode1 to decide whether
//to create code for sun-upscrolling or sun-wanking 

void* spdcadr;
void* spdcadr2;
bool allow_sunsinmove=false;

extern char ghostbyte;/*Must be external, else it would be impossible
  											to declare a variable at a specific adress*/
  		
void init_clowd_delay(){
	CIA2.ta_lo = WAITCYCLES & 255;
	CIA2.ta_hi = WAITCYCLES >>8;
	CIA2.cra = 0x11;
}

void call_paralax(void){
	extern unsigned char* _SPDCODE2_LOAD__;
	
  spdcadr = createspeedcode1(FADECODE);
	spdcadr2 = createspeedcode2();
	
	assert(spdcadr!=0);//The memory must always be free, otherwise stop
	assert(spdcadr2!=0);
	
	init_parscreen();
	sprinit();
	
	asm("sei");
	
	musicinit(0);
    
  CPU_PORT = 0x35;
  irqinit();
  
  asm("cli");
  #if DEBUG == 1
  	while(is_sun_fade());
  #else
  	while(is_sun_fade()){mainloop();}
  #endif
  
  free(spdcadr2);
  free(spdcadr);
  
  spdcadr=createspeedcode1(WANKCODE);
  allow_sunsinmove=true;
  
  #if DEBUG == 1
  	while(true);
  #else
  	while(true){mainloop();}
  #endif
  
  free(spdcadr);
}

void main(void){
	init_clowd_delay();
	
	call_paralax();
}
