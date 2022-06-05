#include "entpack.h"
#include <assert.h>
#ifdef _C64_H 
	#include <c64.h>
#endif
#ifdef C128_H // perhaps i will try that to make compatible
	#include <c128.h>
#endif

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

static unsigned char repeatbyte;
static unsigned int repeatcnt;
//register static unsigned char* from ;
//register static unsigned char* to ;
static int len ;
static unsigned char kennbyte;

unsigned char* entp_amica_part(register unsigned char* from, 
	register unsigned char* to, unsigned int size){
	
	len = size;
    while (len>0){
	if (AMICA_KENNBYTE == *from){
	    // get number of repetitions
		//assert(repeatcnt>1);// no sequenzes of zero or one equal byte 
		//len -= repeatcnt;// decrease number of bytes todepack
		if ((len -=repeatcnt = from[AMICA_REPEAT_OFFSET])<0) {repeatcnt-=len=-len; }
			// does the repeating bytes cross border of color/bitmap area?
			// if yes, then depack only until border is reached
	    repeatbyte = from[AMICA_VALUE_OFFSET];//which byte is repeated?
	    from += AMICA_CODE_LEN;//increace source pointer by
			//len of seq to encrypt repetition
	    memset(to,repeatbyte,repeatcnt);// fill destination with repeating bytes
		to+=repeatcnt;//increase destination
	} else {
	    *to = *from;
	    ++from; ++to;
	    --len;
	}
	}
	return from;
}

unsigned char* entp_zoom_part(register unsigned char* from, 
		register unsigned char* to, unsigned int size){
//just explaining the differences to entp_amica_part
	
	len = size;
    while (len>0){
	--from;
	if (kennbyte == *from){
		from-=(ZOOM_CODE_LEN-1);
		if (!(repeatcnt= from[ZOOM_REPEAT_OFFSET])) repeatcnt = 256;// zoomatic special: repetition count of 
			//zero means 256 times the same byte
		
		if ((len -= repeatcnt)<0) {repeatcnt -=len=-len;}
		repeatbyte = from[ZOOM_VALUE_OFFSET];
	    to -= repeatcnt;
		memset(to,repeatbyte,repeatcnt+1);
	} else {
	    *to = *from;
	    --len;
		--to;
	}
	
    }
return from;
}

int entp_amicapaint(unsigned char* from, unsigned char* to, 
		unsigned char* toscreen, unsigned char* tovideocol, 
		unsigned char* bgcolor){	
			
	unsigned char* adr;
	unsigned char* crot = malloc(256);
	
	adr = entp_amica_part(from, to, E_BITMAPSIZE);//depack bitmap
	if (len>0) memset(toscreen,repeatbyte,len);//depack border crossing bytes 
		//into next area
	adr = entp_amica_part(adr, toscreen+len, E_SCRCOLRAMSIZE-len);//depack scrcol
	if (len>0) memset(tovideocol,repeatbyte,len);
	adr = entp_amica_part(adr, tovideocol+len, E_SCRCOLRAMSIZE-len);//depack colram
	if (len>0) memset(crot,repeatbyte,len);
	adr = entp_amica_part(adr, crot+len, 256-len);//depack color rotation table
	if (len>0) *bgcolor=repeatbyte;
	else *bgcolor = *adr;
	free(crot);
	return EXIT_SUCCESS;
}

int entp_zoomatic(unsigned char* from, unsigned char* to, 
	unsigned char* toscreen, unsigned char* tovideocol,
	unsigned char* bgcolor)
{
	unsigned char* adr;
		
	kennbyte = *from;//see entp_amicapaint for comments
	adr = entp_zoom_part(from, bgcolor, 1);
	if (len>0) memset(tovideocol+(E_SCRCOLRAMSIZE-1)-len,repeatbyte,len);
	// depack even background color(1byte) because it could be encrypted together
	// with other data
	adr = entp_zoom_part(adr, tovideocol+(E_SCRCOLRAMSIZE-1)-len, E_SCRCOLRAMSIZE-len);
	if (len>0) memset(toscreen+E_SCRCOLRAMSIZE-len,repeatbyte,len);
		
	adr = entp_zoom_part(adr, toscreen+(E_SCRCOLRAMSIZE-1)-len, E_SCRCOLRAMSIZE-len);
	if (len>0) memset(to+E_BITMAPSIZE-len,repeatbyte,len);
		
	entp_zoom_part(adr, to+(E_BITMAPSIZE-1)-len, E_BITMAPSIZE-len);

	return EXIT_SUCCESS;
}


