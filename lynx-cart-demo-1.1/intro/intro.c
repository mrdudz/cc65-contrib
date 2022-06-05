#include <lynx.h>
#include <tgi.h>
#include <joystick.h>
#include <conio.h>
#include <6502.h>
#include "../resident/resident.h"
#include "../resident/abcmusic.h"

/*#define tgi_sprite(spr) tgi_ioctl(0, (unsigned)(spr))*/ /* FIXME */

extern char lynxtgi[]; // I am going to link in the tgi driver statically 
extern char lynxjoy[]; // I am going to link in the joy driver statically 

extern void init_irq(void);
extern void install_irq(unsigned char num, unsigned func );
extern void vbl(void);
#define enable_irq(n)\
  asm(" lda #$80\n"\
      " tsb $fd01+"#n"*4\n")	/* enable interrupt of timer n */

extern unsigned char drawPending;
extern unsigned char drawpage;

char a = 1;

char *dummy = "dummy";

int intro(void) 
{
    // Wait for the previous buffer to be on screen
    while (drawPending) ;

    // Make screen black
    tgi_clear();
    drawPending = 1;
    // Wait for the screen to become black
    while (drawPending) ;

#if 1
    while (1) {
	
	// Now we can draw something on the hidden buffer
	tgi_setpalette(tgi_getdefpalette());
	tgi_setcolor(COLOR_WHITE);
	tgi_bar(0,0,159,101);
	tgi_setcolor(COLOR_BLUE);
	tgi_outtextxy(0,0,"Intro screen");
	tgi_outtextxy(0,8, "Not very cool...");
	tgi_outtextxy(0,16, "Created by Karri");
	tgi_outtextxy(0,24, "using the cc65.org");
	tgi_outtextxy(0,32, "compiler");
	tgi_outtextxy(0,48, "Press Opt1 to start");

	// Signal the interrupt routine to display our hidden buffer
	drawPending = 1;
	// Wait until buffer is on screen
	while (drawPending) ;

	// Now we can wait forever until the user presses a key
        while (!kbhit()) {
	    switch (cgetc()) {
            case '1': // Opt1
	        return SKETCH;
	    case '2': // Opt2
	        return RESTART_LYNX;
	    case '3': // Easter egg Opt1 + Opt2
	        return RESTART_LYNX;
	    case '?': // Easter egg Opt1 + Opt2 + Pause
	        return RESTART_LYNX;
	    case 'P': // Pause
	        return RESTART_LYNX;
	    case 'F': // Flip
	        tgi_ioctl(1,0);
	        break;
	    case 'R': // Restart
	        return RESTART_LYNX;
	    }
        }
    }
#endif
    return SKETCH;
} 

void start_irqs(void)
{
    tgi_install(&lynxtgi); // This will activate the Lynx screen 
    joy_install(&lynxjoy); // This will activate the Lynx screen 
    tgi_setdrawpage(drawpage); // Set page for drawing stuff on. Can be 0 or 1.     tgi_init();
    silence();
    init_irq();
    //install_uploader(_9600Bd);
    install_irq(2, (int)&vbl);
    enable_irq(2);
    CLI();
}
