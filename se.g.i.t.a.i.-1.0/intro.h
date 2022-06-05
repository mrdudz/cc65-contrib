#ifndef _INTRO_H
#define _INTRO_H
	#include <logo.h>
	
	extern unsigned int _MUSIC_LOAD__;
	
	#define DD00VALUE (0xc7-(LOGOVICBANK1>>14))
	
	#define SOFTSCROLLX1 5
	#define SOFTSCROLLX2 0
	
	#define TEXTAREAY1 2
	#define TEXTAREAY2 2
	#define TEXTAREAX1 1
	#define TEXTAREAX2 TEXTAREAX1
	#define FRAMEAREAHEIGHT 14
	#define FRAMEAREA1X 0
	#define FRAMEAREA2X FRAMEAREA1X
	#define FRAMEAREA1Y 1
	#define FRAMEAREA2Y 1
	#define FRAMEAREAWIDTH 40
	#define FRAMEAREAFILL 0
	
	#define PARABELTABLEN 128
	#define PARABELLENDIVIDE (256/PARABELTABLEN)
	
	#define PART2DEST (void*)0x0801
	#define PART2START 0x080b

	#define MUSIC_VOLUME (unsigned char*)0x113e
	extern char ghostbyte;/*Must be external, else it would be impossible
  											to declare a variable at a specific adress*/
  												
	void showlogo(void);
	void fastcall fld(unsigned char lines, unsigned char d011mode);
	void waitretrace();
	void fastcall waitrasterline(unsigned char line);
	void creatparabel(unsigned char* dest, unsigned int maxsqrt,
										unsigned char lendivide);
	
	void fastcall music_init(unsigned char tunenr);
	void fastcall music_play(void);
	void fastcall copy2ndpart(void);
#endif
