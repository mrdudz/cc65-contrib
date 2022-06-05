#include <string.h>
#include "speedcode.h"
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>


SPEEDCODE_SU spdcdef_copy;

//*func must not be a zero pointer. If you want to perform no operation, just point to an rts

unsigned char* create_speedcode(SPEEDCODE_SU* spdcdef, void(*dest)(void), bool(*func)(unsigned int reppos) ){
	static unsigned char* saveadr;
	unsigned int i=0;
		
	if (NULL != dest) {
//		(void*)saveadr=dest;
		saveadr=dest;
	}
	//If dest==0, then take the old adress to allow successive speedcodes.
		
	assert(NULL!=func);
	memcpy(&spdcdef_copy, spdcdef, sizeof(spdcdef_copy));
		
	while(func(i++)){
		saveadr = (unsigned char*)memcpy((unsigned char*)saveadr, spdcdef_copy.speedcodepart, spdcdef_copy.size)
		+spdcdef_copy.size;
	}
	*(unsigned char*)saveadr=RTS;
	//Write always RTS, because from func you can also call create_speedcode again,
  //too write modification code for every line
   
	return saveadr;
}
