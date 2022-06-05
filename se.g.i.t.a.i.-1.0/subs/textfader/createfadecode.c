#include <intro.h>
#include <string.h>
#include <textfader.h>

#define RTS_CODE 0x60
#define TAY_CODE 0xa8

void createfadecode(unsigned char* codedest1, 
	unsigned char* codedest2, unsigned char xsize,
	unsigned char ysize){
	
	unsigned char* scradr1 = _LOGOSCRN1_LOAD__+FRAMEAREA1X+(LOGOHEIGHT+FRAMEAREA1Y)*0x28;
	unsigned char* scradr2 = _LOGOSCRN2_LOAD__+FRAMEAREA2X+(FRAMEAREA2Y)*0x28;
		
	unsigned char x;
	unsigned char y;
	
	*codedest1 = *codedest2 = TAY_CODE;
	++codedest1;++codedest2;
	
	for (y = 0; y<ysize; y++){
		for (x = 0; x<xsize; x++){
			fademapelement = _FADETAB_LOAD__[x+y*40];
			fadescradr = scradr1;
			memcpy(codedest1, FADECODESNIBBLE, (unsigned char)&FADECODELEN);
			fadescradr = scradr2;
			memcpy(codedest2, FADECODESNIBBLE, (unsigned char)&FADECODELEN);
			codedest1+=(unsigned char)&FADECODELEN;
			codedest2+=(unsigned char)&FADECODELEN;
			++scradr1;
			++scradr2;
		}
		scradr1+=(40-xsize);
		scradr2+=(40-xsize);
	}
	*codedest1 = RTS_CODE;
	*codedest2 = RTS_CODE;
}
	
	
