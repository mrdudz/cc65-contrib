#ifndef __CHARDISP_H
#define __CHARDISP_H

	#include <stdbool.h>

	typedef struct{
		unsigned char* charset;
		unsigned char xsize;
		unsigned char ysize;
		unsigned char firstchar;
		unsigned char* charoffsets;
	}NXN_CHAR_TYPE;
	
	#define OFFSET1x1 {0}
	#define OFFSETS1x2 {0,128}
	#define OFFSETS2x1 {0,128}
	#define OFFSETS1x2s {0,64}
	#define OFFSETS2x1s {0,64}
	#define OFFSETS2x2_A {0,64,128,192}
	#define OFFSETS2x2_B {0,128,64,192}
	
	void putnxnchar(register NXN_CHAR_TYPE* chardef, 
									unsigned char letter, unsigned char** dest);
	void writetext(unsigned char* textareaadr);
#endif
	