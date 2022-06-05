#include <tgi.h>
#include <conio.h>
#include <stdlib.h>

#undef DYN_DRV

#ifndef DYN_DRV
#define DYN_DRV 0
#endif

#define XSTART 92U
#define YSTART 88U

unsigned int curx=XSTART;
char cury=YSTART,direction=0;

void fd(char len){
	if(len>1){
		switch(direction&3){
			case 1:tgi_lineto(curx+=len,cury);break;
			case 2:tgi_lineto(curx,cury-=len);break;
			case 3:tgi_lineto(curx-=len,cury);break;
			case 0:tgi_lineto(curx,cury+=len);break;
			default:break;
		}
	}else{
		switch(direction&3){
			case 1:tgi_setpixel(curx++,cury);break;
			case 2:tgi_setpixel(curx,cury--);break;
			case 3:tgi_setpixel(curx--,cury);break;
			case 0:tgi_setpixel(curx,cury++);break;
			default:break;
		}
	}
}

void lt(void){
	//direction=(direction+1)&3;
	++direction;
}

void rt(void){
	//direction=(direction-1)&3;
	--direction;
}

void hilbert(unsigned char seglen, unsigned int len){
	unsigned int pos=0;
	
	for(;pos<len;pos++){
		fd(seglen);
		asm("ldy #%o",pos);
		asm("jsr left_or_right");
	}
}

int main(void)
{
#if DYN_DRV
	tgi_load_driver(tgi_stddrv);
	if (tgi_geterror()){
		clrscr();
		cputs("Error loading driver");
		return(EXIT_FAILURE);
	}
#else
	tgi_install(tgi_static_stddrv);
	if (tgi_geterror()){
		clrscr();
		cputs("Error installing driver");
		return(EXIT_FAILURE);
	}
#endif
	tgi_init();
	if (tgi_geterror()){
		clrscr();
		cputs("Error initializing driver");
		return(EXIT_FAILURE);
	}
	tgi_clear();

	tgi_gotoxy(XSTART,YSTART);
	hilbert(1,0x6800);

	cgetc();
#if DYN_DRV
	tgi_unload();
#else
	tgi_uninstall();
#endif
	return EXIT_SUCCESS;
}
