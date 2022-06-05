#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <cbm.h>
#include <assert.h>

//#define debug
/* Switch this on or off, 
dependent on if you want to see debug information*/

#define xsize 40
#define ysize 25

void randfade(unsigned char whichchar, unsigned char color){
	unsigned int randnum, counter, temp;
	int maxrand = xsize*ysize;
	//Set maxrand to size of drawing area
	unsigned int* freepos = malloc(xsize*ysize*sizeof(int));
	/*Table with numbers from 0..xsize*ysize, 
	which represent all the possible drawing positions
	*/
	
	#ifdef debug
		clrscr();
	#endif
	//Clear screen, if debug is on. So debug information is seen better.
	
	revers(1);//Init revers Video
	textcolor(color);//Set textcolor to bordercolor
	
	srand(1);
	//Initialize random number generator with any number 
	
	for (counter = 0; counter < xsize*ysize; counter++){
		freepos[counter] = counter;
	}
	//Initialize Table
	
	do{
		randnum = ((unsigned long int)maxrand*rand())>>15;
		//Get random number in interval [0..maxrand]
		
		--maxrand;
		//Decrease maximum for random numbers
		
		temp = freepos[randnum];
		//Get a drawing position from random number.
		//Avoid unnecessary accesses to array elements.
		
		cputcxy( (temp % xsize), (temp / xsize),whichchar);
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
	}
	while(maxrand>=0);
	
	free(freepos);
}

/*void main(void){
	randfade(' ', 0);
}*/