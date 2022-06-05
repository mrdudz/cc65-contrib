#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

//#define debug
/* Switch this on or off, 
dependent on if you want to see debug information*/

void main(void){
	unsigned int randnum, maxrand, counter, temp;
	unsigned int *freepos;
	/*Table with numbers from 0..xsize*ysize, 
	which represent all the possible drawing positions
	*/
	unsigned char xpos,ypos;

	#ifdef debug
		unsigned char xsize = 16;
		unsigned char ysize = 1;
		//Assume, that almost every screen is larger than this small area
	#else
		unsigned char xsize, ysize;
		screensize(&xsize, &ysize);
		//Assume, that the text area is smaller than 255x255
	#endif
	
	maxrand = xsize*ysize;
	//Set maxrand to size of drawing area
	
	freepos = malloc(xsize*ysize*sizeof(freepos));
	//Init table 
	
	
	#ifdef debug
		clrscr();
	#endif
	//Clear screen, if debug is on. So debug information is seen better.
	//cprintf("Screensize: %d x %d",xsize,ysize);
	//cprintf("%d",xsize*ysize*sizeof(freepos));
	
	srand(1);
	//Initialize random number generator with any number 
	
	for (counter = 0; counter < xsize*ysize; counter++){
		freepos[counter] = counter;
	}
	//Initialize Table
	
	temp = bordercolor(0); bordercolor(temp);
	revers(1);
	textcolor(temp);
	do{
		randnum = ((unsigned long int)maxrand*rand())>>15;
		//Get random number in interval [0..maxrand]
		
		--maxrand;
		//Decrease maximum for random numbers
		
		temp = freepos[randnum];
		xpos = temp % xsize;
		ypos = temp / xsize;
		//Get a drawing position from random number.
		//Avoid unnecessary accesses to array elements.
		
		cputcxy(xpos,ypos,' ');
		//Draw a star
		
		freepos[randnum] = (randnum!=maxrand) ? freepos[maxrand] : *freepos;
		/*No star should be drawn two times at the same position, so
			update table entry with an index on a position without a drawn star.
		*/
		
		#ifdef debug
			gotoxy(0,10);
			cprintf("Star drawn @ pos:%ux  \n\rTable entries:\n\r",randnum);
			for (counter = 0; counter < xsize; counter++){
				cprintf("%d:%ux ",counter,freepos[counter]);
			}
			cputc('#');
		
			while(!kbhit());
			cgetc();
		#endif
		//Print some debug information, if debug is defined
	}while(maxrand>0);
	
	revers(0);
	while(!kbhit());
	cgetc();
	
	free(freepos);
}
