#include <stdbool.h>

#ifndef _LOGO_H
#define _LOGO_H
	#define LOGOVICBANK1 0x4000
	#define LOGOVICBANK2 0x8000
	#define LOGOHEIGHT 9
	#define LOGOWIDTH 40
	#define LOGOVICADDR1 0x80
	//#define LOGOVICADDR2 0x08
	#define LOGOVICADDR2 0x90
	
	#define LOGOCIAVAL1 0xc6
	//#define LOGOCIAVAL2 0xc5
	#define LOGOCIAVAL2 0xc6
	
	#define LOGOY1 0
	#define LOGOY2 16
	#define LOGOBGCOLOR 0
	
	extern unsigned char logocolors[];
	extern unsigned char _LOGOBITMAP1_LOAD__[];
	extern unsigned char _LOGOSCRN1_LOAD__[];
	extern unsigned char _LOGOBITMAP2_LOAD__[];
	extern unsigned char _LOGOSCRN2_LOAD__[];
	extern unsigned char _FRAMEBITMAP1_LOAD__[];
	extern unsigned char _FRAMEBITMAP2_LOAD__[];
	
	extern unsigned char logoscrnarea2[];
	extern unsigned char logobmparea2[];
	
	bool logofader(void);
	bool logomoves(void);
	void allow_logo_move(unsigned char speed);
	void allow_music_fade(void);
	void wait_music_fade(void);
	void copycolram1(void);
	void copycolram2(void);
	void fillarea(unsigned char* start, unsigned int width,
	unsigned int height, unsigned int add, unsigned char value);
#endif