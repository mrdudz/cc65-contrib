#include <string.h>
#include <conio.h>
#include <fontdisp.h>

void putnxnchar(register NXN_CHAR_TYPE* chardef, unsigned char letter, 
								unsigned char** dest){
	unsigned char cx;
	unsigned char cy;
	
	for(cy = 0; cy < chardef->ysize; cy++){
		for(cx = 0; cx < chardef->xsize; cx++){
			memcpy(*dest + (cx<<3) + cy*0x140, 
				chardef->charset+
				 ( (chardef->charoffsets)[cx+cy*(chardef->xsize)]+
				 	letter - chardef->firstchar) *8, 8);
		}
	}
	*dest += cx<<3;
}

	