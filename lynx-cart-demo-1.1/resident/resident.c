/*
 * The code and data defined here is placed in resident RAM
 */
#include <lynx.h>
#include <stdlib.h>
#include <tgi.h>
#include "../resident/resident.h"

unsigned char first_time = 1;
unsigned char next_application = START_IRQS;
extern void start_irqs(void);
extern int intro(void);
extern int sketch(void);
void __fastcall__ FileLoadFile(int filenr);
unsigned char last_application = 0;
int old_stack;

// We don't want any init code here as it would stay in memory forever.
// So just load in the intro segment and jump there to get the system up.
// This will provide us with an interesting challange. The STARTUP-code
// resides in the INTRO_CODE segment. And it will call the routine -main
// after setting up the chips.
// So in a while we will be called again. On the 2nd call we would like to
// pass the execution to our intro-module.
int main(void)
{
    asm("lda _first_time");
    asm("cmp #0");
    asm("beq not_first_time");
    asm("lda #2");
    asm("ldx #0");
    asm("stz _first_time");
    asm("jmp _FileExecFile"); // Start at beginning of intro segment. There is STARTUP_CODE.
    asm("not_first_time:");

    // Choose next application to run
    // It is a good idea to come back here to resident memory
    // to load in the next application.
    while (1) {
        switch (next_application) {
	default:
	case NEXT_APPLICATION:
	    switch (last_application) {
	    default:
		next_application = RESTART_LYNX;
		break;
	    case RESTART_LYNX:
		next_application = SKETCH;
		break;
	    }
	    break;
	case PREV_APPLICATION:
	    switch (last_application) {
	    default:
		next_application = RESTART_LYNX;
		break;
	    case RESTART_LYNX:
		next_application = SKETCH;
		break;
	    }
	    break;
	case START_IRQS:
	    start_irqs();
	    last_application = next_application;
	    next_application = intro();
	    old_stack = *(int *)0;
	    break;
	case RESTART_LYNX:
	    FileLoadFile(INTRO_FILENR);
	    last_application = next_application;
	    next_application = intro();
	    break;
	case SKETCH:
	    FileLoadFile(SKETCH_FILENR);
	    last_application = next_application;
	    next_application = sketch();
	    break;
        }
	if ((*(int *)0) != old_stack)
            next_application = RESTART_LYNX;
    }
}
