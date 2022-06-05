#ifndef __textfader_h
#define __textfader_h

extern unsigned char FADECODESNIBBLE[];
extern unsigned char fademapelement;
extern unsigned char* fadescradr;
extern unsigned char FADECODELEN;
#pragma zpsym("FADECODELEN");
extern unsigned char _FADETAB_LOAD__[];
extern unsigned char _FADECODE1_LOAD__[];
extern unsigned char _FADECODE2_LOAD__[];

void makefadetab(unsigned char xmax, unsigned char ymax,
								 unsigned char offset, register unsigned char* dest);
void createfadecode(unsigned char* codedest1, 
	unsigned char* codedest2, unsigned char xsize,
	unsigned char ysize);
void fastcall fadecode1(unsigned char fadepos);
void fastcall fadecode2(unsigned char fadepos);
void textfader(void);
void allow_text_fade(void);
bool textfades(void);
#endif
