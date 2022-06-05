/* This program uses the property of the inner-product p=x*d1+y*d2 between a direction vector d=(d1,d2) 
	and a position vector r=(x,y), that p is constant on lines perpendicular to d. 
	By calculating p for every pixel and plotting of the value
	as a color you see rotating stripes when rotating the vector d with	a sin table 
	
	Done in April 2006 by Monte Carlos of Cascade. Free use for everybody, but give credits ! */

#include <conio.h>

#define BACKCOLOR COLOR_BLACK
#define BORDERCOLOR COLOR_BLACK

#define SINLEN 32

#define XSIZE 16
#define YSIZE 16
#define SPEED 1
#define CHAR '*'

void main(void){
	static unsigned char x,y;
	static unsigned char sinpos=0;
	static signed int innerprod,temp; // default is signed, but this looks better
	typedef struct 
	{ signed char dx; // default behaviour is unsigned, when using cc65!!!
		signed char dy;
	}DIRECTION;
	
	static DIRECTION d;

	static signed char sinus[] = {-1,24,48,70,90,106,117,125,127,125,117,106,90,70,48,24,
               -1,-25,-49,-71,-91,-107,-118,-126,-128,-126,-118,-107,-91,-71,-49,-25,
								-1,24,48,70,90,106,117,125};
	//must be 1.25 times the same sinustable of a lenght of a power of 2 !!!!

 	bgcolor(BACKCOLOR);
	bordercolor(BORDERCOLOR);
	
	while(!kbhit()){
		temp = innerprod = 0;
		d.dy = sinus[sinpos];
		d.dx = sinus[sinpos+SINLEN/4];
		for(y=0;y<XSIZE;y++){
			gotoxy(0,y);
			for(x=0;x<YSIZE;x++){
				innerprod += d.dx;
				textcolor(innerprod>>8);
				//cputcxy(x,y,CHAR);
				cputc(CHAR);
			}
			innerprod = temp += d.dy;
		}
		sinpos += SPEED;
		sinpos &= SINLEN-1;
	}
}

