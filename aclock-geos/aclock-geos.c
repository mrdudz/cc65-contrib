/*
 * aclock - ascii clock for GEOS
 *
 * Copyright (c) 2002-2004 Antoni Sawicki <tenox@tenox.tc>
 * Version 1.9a (geos 8bit); Dublin, July 2004
 *
 * Compilation: (Requires CC65)
 *				C64: 	 cl65 -O -t geos-cbm aclockres.grc aclock-geos.c -o aclock-c64
 *				Apple2: cl65 -O -t geos-apple aclockres.grc aclock-geos.c -o aclock-apple2
*/

#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <unistd.h>
#include <cc65.h>
#include <geos.h>

#ifdef __GEOS_CBM__
#define FontWH_Ratio 1
#else /* #ifdef __GEOS_CBM__ */
#define FontWH_Ratio 2
#endif /* #ifdef __GEOS_CBM__ */

// GEOS Specific Resource to make it behave more like a GEOS app
struct window wholeScreen = {0, SC_PIX_HEIGHT-1, 0, SC_PIX_WIDTH-1};
void Exit(void)
{
    EnterDeskTop();
}
void Menu = {
    (char)0, (char)14, (int)0, (int)28, (char)(HORIZONTAL|1),
    "quit", (char)MENU_ACTION, &Exit };


int cosa[]={    0, 105, 208, 309, 407, 500, 588, 669, 743, 809, 866, 914, 951, 978, 995,1000, 995, 978, 951, 914, 866, 809, 743, 669, 588, 500, 407, 309, 208, 105,   0,-105,-208,-309,-407,-500,-588,-669,-743,-809,-866,-914,-951,-978,-995,-1000,-995,-978,-951,-914,-866,-809,-743,-669,-588,-500,-407,-309,-208,-105 };
int sina[]={ -1000,-995,-978,-951,-914,-866,-809,-743,-669,-588,-500,-407,-309,-208,-105,   0, 105, 208, 309, 407, 500, 588, 669, 743, 809, 866, 914, 951, 978, 995,1000, 995, 978, 951, 914, 866, 809, 743, 669, 588, 500, 407, 309, 208, 105,   0,-105,-208,-309,-407,-500,-588,-669,-743,-809,-866,-914,-951,-978,-995 };


void cls(void){
	InitDrawWindow(&wholeScreen);
    Rectangle();
	ReDoMenu();
}

void draw_point(unsigned char x, unsigned char y, char p){
	cputcxy(x, y, p);
}


void draw_text(char *p, unsigned char x, unsigned char y){
	cputsxy(x, y, p);
}

void draw_circle(int hand_max, int sXcen, int sYcen){
	int x,y,r;
	char c;

	for(r=0;r<60;r++){
		x=(cosa[r]*hand_max*FontWH_Ratio/1000)+sXcen;
		y=(sina[r]*hand_max/1000)+sYcen;
		switch (r) {
			case 0:
			case 5:
			case 10:
			case 15:
			case 20:
			case 25:
			case 30:
			case 35:
			case 40:
			case 45:
			case 50:
			case 55:
				c='o';
				break;
			default:
				c='.';
				break;
		}
		draw_point(x,y,c);
	}
}

void draw_hand(int minute, int hlenght, char c, int sXcen, int sYcen){
	int x,y,n;


	for(n=1; n<hlenght; n++){
		x=(cosa[minute]*n*FontWH_Ratio/1000)+sXcen;
		y=(sina[minute]*n/1000)+sYcen;
		draw_point(x,y,c);
	}
}

void main(void) {
	char info[]="Written by Antoni Sawicki <tenox@tenox.tc>\nVersion 1.9a; Dublin, Mar 2004\n";
	char digital_time[15];
	unsigned char sXmax, sYmax, smax, hand_max, sXcen, sYcen;
	time_t t;
	struct tm *ltime;
	#ifdef __GEOS_CBM__
	char tv_type;
	#endif
	
	// GEOS Gui Init
	SetPattern(0);
	DoMenu(&Menu);
	#ifdef __GEOS_CBM__
	tv_type = get_tv();
	#endif
	
	screensize(&sXmax, &sYmax);
	
	if(sXmax/FontWH_Ratio <= sYmax)
		smax=sXmax/FontWH_Ratio;
	else
		smax=sYmax;

	hand_max = (smax/2)-1;

	sXcen = sXmax/2;
	sYcen = sYmax/2;
	
	
   	while (1) {
		time(&t);
		ltime=localtime(&t);
		
		cls();
		draw_circle(hand_max, sXcen, sYcen);
		
		draw_hand((system_date.s_hour%12*5)+(system_date.s_minutes/12), 2*hand_max/3, 'h', sXcen, sYcen);
		draw_hand(system_date.s_minutes, hand_max-2, 'm', sXcen, sYcen);
		draw_hand(system_date.s_seconds, hand_max-1, '.', sXcen, sYcen);

		draw_text(".:ACLOCK:.", sXcen-5, sYcen-(3*hand_max/5));
		sprintf(digital_time, "[%02d:%02d:%02d]", system_date.s_hour, system_date.s_minutes, system_date.s_seconds);
		draw_text(digital_time, sXcen-5, sYcen+(3*hand_max/5));
		
		// Return control to GEOS
		#ifdef __GEOS_CBM__
		Sleep(1*(tv_type==TV_PAL)?50:60); // time is in jiffies (either 50 or 60 depending on pal/ntsc)
		#else /* #ifdef __GEOS_CBM__ */
		Sleep(1*60); // time is in jiffies (either 50 or 60 depending on pal/ntsc)
		#endif /* #ifdef __GEOS_CBM__ */
		
	}


	return;

}