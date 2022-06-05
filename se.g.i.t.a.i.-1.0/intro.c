#include <conio.h>
#include <stdlib.h>
#include <cbm.h>
#include <string.h>
#include <intro.h>
#include <stdbool.h>
#include <irq.h>
#include <fontdisp.h>
#include <textfader.h>
#include <kickborder.h>
#include <randfade.h>
#include <time.h>
#include <misc.h>

#define MUSIC_DELAY (10*CLOCKS_PER_SEC/3)
#define ZP_SIZE 256

unsigned char* parabeltab;
bool quit = false;
//irq tests for pressed space key, so it is possible
//to quit intro before the text ends
unsigned char* zp;

extern unsigned char _FADETAB_LOAD__[];

extern unsigned char _LOGOBITMAP1_LOAD__[];
extern unsigned char _LOGOBITMAP2_LOAD__[];
extern unsigned char _INTROCHARSET_LOAD__[];
extern unsigned int _INTROCHARSET_SIZE__;
extern unsigned char _PART2_LOAD__[];
extern unsigned int _PART2_SIZE__;
extern unsigned char _COPYCODE_LOAD__[];
extern unsigned char _COPYCODE_RUN__[];
extern unsigned int _COPYCODE_SIZE__[];
extern unsigned char _ZEROPAGE_LOAD__[];
extern unsigned char _ZEROPAGE_SIZE__[];

void basicfadeoff(){
	randfade(' ', LOGOBGCOLOR);
	VIC.bgcolor0 = LOGOBGCOLOR;
	clrscr();
	
	ghostbyte = 0;

	kickborder();
}

void initscreen(void){
	VIC.ctrl1 = 0;
	VIC.bordercolor = 0;
	VIC.bgcolor0 = 0;
	
	waitretrace();	
	//Set VIC-Registers to show MC-bitmap
	CIA2.pra = DD00VALUE;
	VIC.addr = LOGOVICADDR1;
	VIC.ctrl2 = 0x18+SOFTSCROLLX1;
	
	showlogo();
}

void initall(void){
	music_init(0);
	
	initscreen();
	
	makefadetab(40,16,5,_FADETAB_LOAD__);

	createfadecode(_FADECODE1_LOAD__, _FADECODE2_LOAD__, 
							 FRAMEAREAWIDTH, FRAMEAREAHEIGHT);
	
	parabeltab = malloc(PARABELTABLEN);
	creatparabel(parabeltab, 227, PARABELLENDIVIDE);//227 is 16*sqrt(250)	
	
	switch_raster_irq(0x25, &rasterirq);

	//prepare_font();
		
	irqinit();
}

/*
void prepare_font(void){
	//This routine is only needed, when using a 
	//multicolor charset on a background with
	//nonzero two-bit combination
	unsigned char* charsetptr=_INTROCHARSET_LOAD__;
	unsigned int chroffset=0;
	
	for(; chroffset < (unsigned int)&_INTROCHARSET_SIZE__;chroffset++){
		*charsetptr = (*charsetptr^0xff&0x55)|(*charsetptr&0xaa);
		++charsetptr;
	}
}
*/

void switchscreen(void){
	static unsigned char scrnumber=0;
	
	VIC.addr = scrnumber?LOGOVICADDR1:LOGOVICADDR2;
	
	scrnumber ^=1;
}
	
int main(void){
	asm("sei");
	
	zp = malloc(256);
	savezp(zp);//later decrunch needs restored zeropage

	basicfadeoff();
	
	initall();
	asm("cli");

	delay_loop(MUSIC_DELAY);
	//Wait until music-intro is over
	
	waitrasterline(250);
	allow_logo_move(2);
		
	//Logo moves up the first time
	while( logomoves() );
	if(quit){
		//if space is pressed before first text-screen
		allow_music_fade();
		allow_logo_move(1);
		while( logomoves() );
	}else{
		allow_text_fade();
		while( textfades() );

		writetext(_FRAMEBITMAP1_LOAD__+TEXTAREAY1*0x140+
											 (TEXTAREAX1*8));
		
		creatparabel(parabeltab, 182, PARABELLENDIVIDE);//182 is 16*sqrt(128)	

		do{
			allow_text_fade();
		
			while( textfades() );
				
			memcpy(_LOGOBITMAP2_LOAD__, _LOGOBITMAP1_LOAD__, 
				LOGOHEIGHT*0x140);
		  if(quit){
				creatparabel(parabeltab, 227, PARABELLENDIVIDE);//227 is 16*sqrt(250)
				allow_music_fade();
			}
			/*Logo moves down*/
			allow_logo_move(1);
			while( logomoves() );
			if (quit) {break;}
			
			/*Switch to screen with logo at bottom*/
			waitrasterline(0);
			switchscreen();
			copycolram1();
			//copying must be fast, so use asm
			fillarea(_FRAMEBITMAP2_LOAD__+(FRAMEAREA2X*8)+(FRAMEAREA2Y*0x140),
							 FRAMEAREAWIDTH*8, FRAMEAREAHEIGHT, 0x140, FRAMEAREAFILL);
			memset(_FRAMEBITMAP2_LOAD__+(FRAMEAREAHEIGHT*0x140), 
						 0, (25-FRAMEAREAHEIGHT-LOGOHEIGHT)*0x140);
			
			allow_text_fade();
			while( textfades() );
			
			writetext(_FRAMEBITMAP2_LOAD__+(TEXTAREAY2*0x140)+
											 (TEXTAREAX2*8));
				
			allow_text_fade();
			while( textfades() );
			
			memcpy(_LOGOBITMAP1_LOAD__, _LOGOBITMAP2_LOAD__, 
				LOGOHEIGHT*0x140);
				
			waitrasterline(250-(LOGOHEIGHT*8));
			/*Logo moves up*/
			allow_logo_move(1);
			copycolram2();
			//copying must be fast, so use asm
			waitrasterline(0);
			/*Switch to screen with logo at top*/
			switchscreen();
			memset(COLOR_RAM+(LOGOY2*0x28), 0, LOGOHEIGHT*SCRW);
			//don't use asm here, because we have nearly whole 
			//time of logo-movement to clear color-ram
			
			while( logomoves() );
			
			if(quit){
				creatparabel(parabeltab, 227, PARABELLENDIVIDE);//227 is 16*sqrt(250)
				allow_music_fade();
				allow_logo_move(1);
				while( logomoves() );
				break;
			}
			
			//Fill Textbackground with pattern for MC1
			memset(_FRAMEBITMAP1_LOAD__, 0, FRAMEAREA1Y*0x140);
			fillarea(_FRAMEBITMAP1_LOAD__+(FRAMEAREA1X*8)+(FRAMEAREA1Y*0x140),
							 FRAMEAREAWIDTH*8, FRAMEAREAHEIGHT, 0x140, FRAMEAREAFILL);
					
			allow_text_fade();
			while( textfades() );
	
			writetext(_FRAMEBITMAP1_LOAD__+TEXTAREAY1*0x140+
											 (TEXTAREAX1*8));
		}while(true);
	}
	
	VIC.ctrl1 = 0;
	wait_music_fade();
	asm("sei");
	stop_irq();

	free(parabeltab);

	memcpy(_COPYCODE_RUN__, _COPYCODE_LOAD__, (unsigned int)_COPYCODE_SIZE__);
	//The routine, which copies the packed 2nd part 
	//to $0801 must be in lower-ram to avoid beeing
	//overwritten while decrunching.
	restorezp(zp);
	free(zp);
	copy2ndpart();
}
