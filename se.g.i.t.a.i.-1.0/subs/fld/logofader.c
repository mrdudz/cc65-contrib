#include <stdbool.h>
#include <intro.h>
#include <cbm.h>
#include <stdlib.h>

extern unsigned char* parabeltab;
//defined in main program

/* Flag for logo-movement. True if logo allowed to move, false if it is not.*/
bool move=false;

signed char direction = -1;
unsigned char deltalasttwo;
//delta last two is the difference between the last
//two elements of the parabel tab to assure soft
//additional fade-off when the space key is pressed
//during logo down movement

bool logofader(void){
	static int i=PARABELTABLEN-1;
			
	unsigned char numfldlines;
	extern bool quit;

	if(move){
		if(i<PARABELTABLEN){
			//i in the limits of parabeltab
			//normal logo-movement
			numfldlines = parabeltab[(unsigned char)i];
		}else{
			numfldlines+=deltalasttwo;
			//i further than last element of parabeltab
			//->logo is moving down and additional fadeoff
			//  is needed
		}
		fld(numfldlines, numfldlines<(200-72)?0x38:0x30);
		//If logo moves into upper screen area, then
		//make upper/lower border small else make it
		//wide for smooth scrolling out of the bottom
		i+=direction;
	
		if (i<0){
			//Check, if first element of parabeltab reached
			++i;
			direction = -direction;
			move = false;
		}else{
			if (i==PARABELTABLEN){
				//Check, if last element of parabeltab reached
				if ((!quit)||(parabeltab[PARABELTABLEN-1]>192)){
					--i;
					direction = -direction;
					move = false;
				}
				//If space not pressed or already fade-off,
				//then set move to false
			}else{
				if ( numfldlines >= 200 )
					 {move = false;
					 	allow_music_fade();}
				//Additional fade off, if space pressed 
				//during logo down movement
			}
		}

		return move;
	}else{
		return direction>0?true:false;
	}
}

/* Returns, if logo is moving or not */
bool logomoves(void){
	return move;
}

void allow_logo_move(unsigned char speed){
	direction = direction>0?speed:-speed;
	move = true;
	deltalasttwo = parabeltab[PARABELTABLEN-1]-
							   parabeltab[PARABELTABLEN-2];
}
