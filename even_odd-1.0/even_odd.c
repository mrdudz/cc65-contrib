#include <conio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define XSIZE 20
#define YSIZE 15

int main(int argc, char* argv[]){
	int8_t x,y;
	int8_t xsize,ysize;
	uint8_t maxx, maxy;
	
	uint8_t coltable[] = {COLOR_BLUE,
		COLOR_PURPLE,
		COLOR_LIGHTBLUE,
		COLOR_CYAN,
		COLOR_LIGHTGREEN,
		COLOR_WHITE,
		COLOR_LIGHTGREEN,
		COLOR_CYAN,
		COLOR_LIGHTBLUE,
		COLOR_PURPLE};
	
	clrscr();
	
	if (argc!=3){
		cprintf("*** Diamond-Effect ***\n\r"
					 "Compiled with CC65 Version %d.%d.%d.%d\n\r"
					"Wrong number of parameters (%d instead of 2)\n\r"
					"Use (Space is important):\n\rrun:rem X-Size, Y-Size",__CC65__>>12,
					(__CC65__>>8)&15,(__CC65__>>4)&15,__CC65__&15,argc-1);
		exit(EXIT_FAILURE);
	}
	
	bgcolor(0);
	bordercolor(0);
	
	screensize(&maxx, &maxy);
	
	xsize = atoi(argv[1])-1;
	ysize = atoi(argv[2])-1;
	
	if (xsize >= maxx){cputs("X-size too large\n\r");exit(EXIT_FAILURE);}
	if (ysize >= maxy){cputs("Y-size too large\n\r");exit(EXIT_FAILURE);}
	
	for (x = -xsize; x<=xsize; x+=2){
		for (y = -ysize; y<=ysize; y+=2){
			gotoxy((x+xsize)/2, (y+ysize)/2);
			textcolor(coltable [ (abs(x) + abs(y))/2%sizeof(coltable)] );
			cputc('*');
		}
	}
	
	while(!kbhit());
	
	return 0;
}

	
