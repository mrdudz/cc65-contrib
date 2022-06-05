/*****************************************************************************
* TETRIS for Commodore PET (using cc65 compiler)
* (c) Tim Howe, March 2010
*
*****************************************************************************/

/* platform.c */

#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "tetris.h"
#include "platform.h"


/*****************************************************************************
* Clear the screen and draw the borders and other 'static' parts of the
*   display
*****************************************************************************/
void configure_screen(void)
{
    UINT8 i;
    UINT8 * scr;

    clrscr();

    /* Draw the outer border */
    scr = (UINT8 *)SCREEN;
    scr[0] = BORDER_TL;
    scr[SCREEN_WIDTH - 1] = BORDER_TR;
    for (i=SCREEN_WIDTH - 2; i>0; i--)
    {
        scr[i] = BORDER_TOP;
    }

    scr += SCREEN_WIDTH;
    for (i=SCREEN_HEIGHT - 2; i>0; i--)
    {
        scr[0] = BORDER_LEFT;
        scr[SCREEN_WIDTH - 1] = BORDER_RIGHT;
        scr += SCREEN_WIDTH;
    }

    scr[0] = BORDER_BL;
    scr[SCREEN_WIDTH - 1] = BORDER_BR;
    for (i=SCREEN_WIDTH - 2; i>0; i--)
    {
        scr[i] = BORDER_BOTTOM;
    }

    /* Draw the game area border */
    scr = (UINT8 *)(SCREEN + GAME_SCR_POS - SCREEN_WIDTH - 1);
    for (i=BOARD_X; i>0; i--)
    {
        scr[i] = LINE_BOT;
    }

    scr += SCREEN_WIDTH;
    for (i=BOARD_Y; i>0; i--)
    {
        scr[0] = LINE_RIGHT;
        scr[BOARD_X + 1] = LINE_LEFT;
        scr += SCREEN_WIDTH;
    }

    for (i=BOARD_X; i>0; i--)
    {
        scr[i] = LINE_TOP;
    }

    /* Plot the static text areas */
    gotoxy(SCORE_X, SCORE_Y-2);
    cputs("score:");
    gotoxy(SCORE_X, SCORE_Y);
    cputs("000000");

    gotoxy(LEVEL_X-6, LEVEL_Y);
    cputs("level  0");

    gotoxy(NEXT_X-1, NEXT_Y-2);
    cputs("next:");

    /* Print the key mapping */
    gotoxy(KEYS_X, KEYS_Y+2);
    cputs("left  - 4");
    gotoxy(KEYS_X, KEYS_Y+4);
    cputs("right - 6");
    gotoxy(KEYS_X, KEYS_Y+6);
    cputs("down  - 2");
    gotoxy(KEYS_X, KEYS_Y+8);
    cputs("rot l - z");
    gotoxy(KEYS_X, KEYS_Y+10);
    cputs("rot r - x");
    gotoxy(KEYS_X, KEYS_Y+12);
    cputs("drop  - sp");
    gotoxy(KEYS_X, KEYS_Y+14);
    cputs("quit  - q");

    /* Print an empty high score table */
    gotoxy(HIGHSC_X, HIGHSC_Y);
    cputs("high scores:");
}



/* Display the current score */
void print_score(UINT16 sc)
{
    gotoxy(SCORE_X, SCORE_Y);
    print_uint16_5(sc);
}


/* Display the current level */
void print_level(UINT8 lev)
{
    gotoxy(LEVEL_X, LEVEL_Y);
    print_uint8_2(lev);
}



/*****************************************************************************
* Display the GAME OVER box
*****************************************************************************/
const UINT8 gameover[4][7] =
{
    { 0xB0, 0xC0, 0xC0, 0xC0, 0xC0, 0xAE, 0x00 },
    { 0xDD, 0x47, 0x41, 0x4D, 0x45, 0xDD, 0x00 },
    { 0xDD, 0x4F, 0x56, 0x45, 0x52, 0xDD, 0x00 },
    { 0xAD, 0xC0, 0xC0, 0xC0, 0xC0, 0xBD, 0x00 }
};

void print_game_over(void)
{
    /* I had trouble defining PET graphics chars with \x?? within C strings,
       hence the binary array above. */
    gotoxy(GAMEOVER_X, GAMEOVER_Y);
    cputs(&gameover[0][0]);
    gotoxy(GAMEOVER_X, GAMEOVER_Y+1);
    cputs(&gameover[1][0]);
    gotoxy(GAMEOVER_X, GAMEOVER_Y+2);
    cputs(&gameover[2][0]);
    gotoxy(GAMEOVER_X, GAMEOVER_Y+3);
    cputs(&gameover[3][0]);
}


/* Display the high score table */
void print_highscores(HIGHSCORE *high_scores)
{
    UINT8 i;

    for (i=0; i<NUM_HIGH_SCORES; i++)
    {
        gotoxy(HIGHSC_X, HIGHSC_Y+2+(i<<1));
        print_uint16_5(high_scores[i].score);
        gotoxy(HIGHSC_X+5, HIGHSC_Y+2+(i<<1));
        cputs("0 ");
        cputs(high_scores[i].name);
    }
}


#ifndef TETRIS_8K
/*****************************************************************************
* Display the initial game splash screen and cresits
*****************************************************************************/
UINT8 splash_P[] =
{
    0xA4, 0xA4, 0xA4, 0x11, 0x9D, 0x9D, 0x9D,
    0xA5, 0x20, 0x20, 0xCD, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA5, 0xD5, 0xC9, 0xA7, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA5, 0xCA, 0xCB, 0xA7, 0X11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA5, 0x20, 0x20, 0xCE, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA5, 0xCF, 0xA3, 0x11, 0x9D, 0x9D, 0x9D,
    0xA5, 0xA5, 0x11, 0x9D, 0x9D,
    0xCC, 0xA5, 0x00
};

UINT8 splash_e[] =
{
    0x20, 0xA4, 0xA4, 0x11, 0x9D, 0x9D, 0x9D,
    0xCE, 0xD5, 0xC9, 0xCD, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA5, 0xCA, 0xCB, 0xCE, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA5, 0x20, 0xCE, 0X11, 0x9D, 0x9D, 0x9D,
    0xCD, 0x20, 0xA3, 0xD0, 0x11, 0x9D, 0x9D, 0x9D,
    0xA3, 0xA3, 0xA3, 0x00
};

UINT8 splash_t[] =
{
    0xA4, 0x11, 0x9D, 0x9D,
    0xBA, 0x20, 0xCC, 0xA4, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xCC, 0x20, 0xA4, 0xBA, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA7, 0x20, 0xA5, 0x11, 0x9D, 0x9D, 0x9D,
    0xA7, 0x20, 0xCD, 0xA4, 0x11, 0x9D, 0x9D, 0x9D,
    0xCD, 0xA4, 0xBA, 0x00
};

UINT8 splash_T[] =
{
    0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0x11, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
    0xCC, 0xA4, 0x20, 0xA4, 0xBA, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA7, 0x20, 0xA5, 0x11, 0x9D, 0x9D, 0x9D,
    0xA7, 0x20, 0xA5, 0x11, 0x9D, 0x9D, 0x9D,
    0xA7, 0x20, 0xA5, 0x11, 0x9D, 0x9D, 0x9D,
    0xA7, 0x20, 0xA5, 0x11, 0x9D, 0x9D, 0x9D,
    0xA7, 0x20, 0xA5, 0x11, 0x9D, 0x9D, 0x9D,
    0xA7, 0xA4, 0xA5, 0x00
};

UINT8 splash_r[] =
{
    0xA4, 0x20, 0xA4, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA7, 0xA7, 0xCE, 0xBA, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA7, 0x20, 0xCE, 0x11, 0x9D, 0x9D, 0x9D,
    0xA7, 0x20, 0xA5, 0x11, 0x9D, 0x9D, 0x9D,
    0xA7, 0xA4, 0xA5, 0x00
};

UINT8 splash_i[] =
{
    0xA4, 0xA4, 0xA4, 0x11, 0x9D, 0x9D, 0x9D,
    0xCC, 0x20, 0xBA, 0x11, 0x9D, 0x9D, 0x9D,
    0xA7, 0x20, 0xA5, 0x11, 0x9D, 0x9D, 0x9D,
    0xBA, 0x20, 0xCC, 0x11, 0x9D, 0x9D, 0x9D,
    0xCC, 0xA4, 0xBA, 0x00
};

UINT8 splash_s[] =
{
    0xA4, 0xA4, 0x11, 0x9D, 0x9D, 0x9D,
    0xCE, 0x20, 0xA4, 0xCD, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xCD, 0x20, 0xCD, 0xA3, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xA4, 0xCD, 0x20, 0xCD, 0x11, 0x9D, 0x9D, 0x9D, 0x9D,
    0xCD, 0xA3, 0x20, 0xCE, 0x11, 0x9D, 0x9D, 0x9D,
    0xA3, 0xA3, 0x00
};

void print_splash_screen(void)
{
    /* I had trouble defining PET graphics chars with \x?? within C strings,
       hence the binary array above. Also, cputs() seems incapable of
       processing inline cursor movement characters, hence the use of
       a hand-coded print_string() function. */
    clrscr();

    gotoxy(13, 0);
    print_string(splash_P);
    gotoxy(18, 3);
    print_string(splash_e);
    gotoxy(24, 2);
    print_string(splash_t);
    gotoxy(7, 8);
    print_string(splash_T);
    gotoxy(11, 11);
    print_string(splash_e);
    gotoxy(17, 10);
    print_string(splash_t);
    gotoxy(21, 11);
    print_string(splash_r);
    gotoxy(25, 11);
    print_string(splash_i);
    gotoxy(30, 11);
    print_string(splash_s);

    gotoxy(2, 19);
    cputs("version 1.3   by tim howe, march 2010");
    gotoxy(2, 20);
#ifdef DAC_TETRIS
    cputs("sound effects via dac on the user port");
    gotoxy(5, 21);
#endif
    cputs("(email: timbly123@ntlworld.com)");
    gotoxy(12, 23);
    cputs("for ben and ethan");
}
#endif


/*****************************************************************************
* Initialise the high score table if one is not already found in memory
*****************************************************************************/
const HIGHSCORE new_high_scores[NUM_HIGH_SCORES] =
{   { 1500U, "tdh" },
    { 500U,  "bah" },
    { 500U,  "ejh" },
    { 0, "" },
    { 0, "" },
    { 0, "" },
    { 0, "" }
};

void init_high_scores(void)
{
    /* The high score table is stored at the beginning of the cass #2
       buffer. This makes it somewhat less volatile than it would be
       if it were located in .bss. The table may also be saved from the
       TIM monitor: .S"HS",08,033A,037A. Clunky but works. */

    /* Check for presence of GUID */
    if (*((UINT16 *)HIGHSCORE_GUID_MEM) != HIGHSCORE_GUID)
    {
        /* Not present, so initialise with a clean table */
        memcpy((HIGHSCORE *)HIGHSCORE_MEM, new_high_scores,
            NUM_HIGH_SCORES*sizeof(HIGHSCORE));

        *((UINT16 *)HIGHSCORE_GUID_MEM) = HIGHSCORE_GUID;
    }
}


/* Read and return debounced keyboard matrix value */
UINT8 read_keyboard(void)
{
    return *((UINT8 *)KEYB_MATRIX);
}


/* Clear the keyboard buffer */
void flush_keyboard_buffer(void)
{
    *((UINT8 *)KEYB_BUF_NUM_CHARS) = 0;
}


/* Wait for 'delay' jiffies */
void jiffy_wait(UINT16 delay)
{
    UINT16 start = get_time();
    while (get_time() - start < delay);
}


#ifdef DAC_TETRIS
void reset_user_port(void)
{
    /* Set user port direction to all inputs */
    *((UINT8 *)0xE843) = 0;
}
#endif

//*((UINT8 *)0xE84C) = 12; // Force graphics char set


/* EOF */
