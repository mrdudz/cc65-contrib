/*
 *  Copyright (c) 2004, Dennis Kuschel.
 *  All rights reserved. 
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission. 
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 *  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


/**
 * @file    snakes.c
 * @brief   snake multitasking demo for Commodore 64
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: snakes.c,v 1.1.1.1 2004/03/22 21:05:09 dkuschel Exp $
 */

#include <stdlib.h>
#include <picoos.h>


/*---------------------------------------------------------------------------
 *  DEFINES AND MACROS
 */

#define MAX_SNAKES          10
#define MAX_SNAKE_LEN       20
#define MIN_SNAKE_LEN       12
#define VAR_SNAKE_LEN       (MAX_SNAKE_LEN - MIN_SNAKE_LEN)
#define PRIO_MAINPROG       3
#define PRIO_SNAKE1         2
#define PRIO_SNAKE2         1

#define CHAR_SNAKE_HEAD     37
#define CHAR_SNAKE_BODY     15
#define CHAR_SNAKE_NONE     32
#define CHAR_FRAME          160

#define COLOR_FRAME         11
#define COLOR_TEXT          14

#define SCREEN_MAXX         40
#define SCREEN_MAXY         25
#define SCREEN_SIZE         (SCREEN_MAXX * SCREEN_MAXY)

#define SETSCREENCOLOR(c)   do { *((char*)((void*)0xD020)) = c; \
                                 *((char*)((void*)0xD021)) = c; } while(0);
#define SETTEXTCOLOR(c)     *((char*)((void*)0x0286)) = c
#define PUTCOLOR(adr,val)   *(((char*)((void*)0xD800))+adr) = val
#define PUTSCREEN(adr,val)  *(((char*)((void*)0x0400))+adr) = val
#define GETSCREEN(adr)      *(((char*)((void*)0x0400))+adr)

#define SNAKECOLOR(s)       snakecolors_g[(s) % MAX_SNAKECOLORS]
#define ADD_ADDR_X(d)       ((signed int)(d))
#define ADD_ADDR_Y(d)       ((signed int)_stepy[1+(d)])
#define DIR2STEPX(d)        _dirsx[d]
#define DIR2STEPY(d)        _dirsy[d]
#define INVERTDIR(d)        ((d) ^ 1)
#define DIRWALKCHAR(olddir, newdir)  _dirchar[olddir][newdir]
/* directions:  0 = right, 1 = left, 2 = down, 3 = up */



/*---------------------------------------------------------------------------
 *  TYPEDEFS
 */

typedef struct snake
{
  signed   int      addr[MAX_SNAKE_LEN];
  unsigned char     dir[MAX_SNAKE_LEN];
  signed   char     stepx, stepy;
  unsigned char     curdir, lastdir;
  unsigned char     head, tail;
  unsigned char     speed; /* steps per second */
  unsigned char     maxlen;
  unsigned char     color;
  unsigned char     holdcntr;
  unsigned char     seccntr;
  unsigned char     excitement;
} snake_t;



/*---------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 */

POSSEMA_t     randsem_g;
POSSEMA_t     screensem_g;
POSSEMA_t     joinsema_g;
unsigned char stopflag_g;
signed   char snakecounter_g;
JIF_t         nextjiffies_g;

signed   char _stepy[3]   = {-40, 0, 40};
signed   char _dirsx[4]   = {1, -1,  0,  0};
signed   char _dirsy[4]   = {0,  0,  1, -1};

unsigned char _dirchar[4][4] =
{
  {64,   64, 110, 125},
  {64,   64, 112, 109},
  {109, 125,  93,  93},
  {112, 110,  93,  93}
};

#define MAX_SNAKECOLORS  8
unsigned char snakecolors_g[MAX_SNAKECOLORS] = {14, 10, 7, 5, 8, 3, 6, 12};



/*---------------------------------------------------------------------------
 *  FUNCTION PROTOTYPES
 */

unsigned int  random(unsigned int maxrand);
void          mainfunc(void *arg);
void          printFrame(void);
void          newSnake(char speed, char length, char excitement);
void          snakeTask(void *arg);
void          step(snake_t *sn);
void          walk(snake_t *sn);
unsigned char teststep(snake_t *sn, signed char sx, signed char sy);
unsigned char isBarrier(snake_t *sn, signed int p);
void          printCPUusage(void);
void          printNmbrOfSnakes(void);
char          getkey(void);



/*---------------------------------------------------------------------------
 *  SETUP PICO]OS  (with nano layer option)
 */

#define HEAPSIZE 0x2000
static char membuf_g[HEAPSIZE];
void *__heap_start  = (void*) &membuf_g[0];
void *__heap_end    = (void*) &membuf_g[HEAPSIZE-1];

void main(void)
{
  nosInit(mainfunc, NULL, PRIO_MAINPROG, 0, 0);
}



/*---------------------------------------------------------------------------
 *  FUNCTION IMPLEMENTATION
 */


/* Get random number. This function is thread save.
 */
unsigned int random(unsigned int maxrand)
{
  unsigned int r;
  posSemaGet(randsem_g);
  r = (unsigned int) rand() % maxrand;
  posSemaSignal(randsem_g);
  return r;
}



/* The snake does a single step on the screen.
 */
void step(snake_t *sn)
{
  unsigned char h = sn->head;
  unsigned char t = sn->tail;
  signed   char sx= sn->stepx;
  signed   char sy= sn->stepy;
  signed   int  a = sn->addr[h];

  PUTSCREEN(a,  DIRWALKCHAR(sn->dir[h], sn->curdir));

  a += ADD_ADDR_X(sx) + ADD_ADDR_Y(sy);

  if (++h >= sn->maxlen)
    h = 0;

  if (h == t)
  {
    PUTSCREEN(sn->addr[h], CHAR_SNAKE_NONE);
    if (++t >= sn->maxlen)
      t = 0;
    sn->tail = t;
  }

  sn->addr[h]  = a;
  sn->lastdir  = sn->dir[t];
  sn->dir[h]   = sn->curdir;
  sn->head     = h;
  sn->holdcntr = MAX_SNAKE_LEN;

  PUTCOLOR (a, sn->color);
  PUTSCREEN(a, CHAR_SNAKE_HEAD);
}



/* Test if the snake can step into the wished direction.
 */
unsigned char teststep(snake_t *sn, signed char sx, signed char sy)
{
  unsigned char h = sn->head;
  signed   int  a;

  a = sn->addr[h] + ADD_ADDR_X(sx) + ADD_ADDR_Y(sy);
  return (GETSCREEN(a) == CHAR_SNAKE_NONE) ? 1 : 0;
}



/* Test if the snake reaches an invincible barrier
 * that would in a snake dead lock.
 */
unsigned char isBarrier(snake_t *sn, signed int a)
{
  signed char i;

  if (GETSCREEN(a) == CHAR_FRAME)
    return 1;  /* we have reached the screen boarder */

  for (i = sn->maxlen-1; i >= 0; i--)
  {
    if (sn->addr[i] == a)
      return 1;  /* wie just bit us */
  }
  return 0;
}



/* Reverse walking direction.
 */
void goback(snake_t *sn)
{
  signed   int  abuf[MAX_SNAKE_LEN];
  unsigned char dbuf[MAX_SNAKE_LEN];
  unsigned char d;
  unsigned char h = sn->head;
  unsigned char t = sn->tail;
  signed   char i;

  d = sn->dir[h];

  i = (signed char) h - (signed char) t;
  if (i < 0) i += (signed char) sn->maxlen;
  sn->head = (unsigned char) i;
  sn->tail = 0;

  for ( ; i >= 0; i--)
  {
    abuf[i] = sn->addr[t];
    dbuf[i] = INVERTDIR(sn->dir[t]);
    if (++t >= sn->maxlen)
      t = 0;
  }

  nosMemCopy(sn->addr, abuf, MAX_SNAKE_LEN * sizeof(signed int));
  nosMemCopy(sn->dir+1 , dbuf, MAX_SNAKE_LEN-1);
  sn->dir[0] = dbuf[0];

  sn->curdir = INVERTDIR(sn->lastdir);
  sn->lastdir= INVERTDIR(d);
  sn->stepx  = DIR2STEPX(sn->curdir);
  sn->stepy  = DIR2STEPY(sn->curdir);

  PUTSCREEN(sn->addr[sn->head], CHAR_SNAKE_HEAD);
  PUTSCREEN(sn->addr[sn->tail], DIRWALKCHAR(d, d));
}



/* Walk the snake.
 */
void walk(snake_t *sn)
{
  unsigned char o, d, i;
  signed   char sx, sy;
  signed   int  a;

  /* get exclusive access to the screen */
  posSemaGet(screensem_g);

  if (random(sn->excitement) != 0)
  {
    /* if we can, we go straight forward */
    if (teststep(sn, sn->stepx, sn->stepy) != 0)
    {
      step(sn);
      posSemaSignal(screensem_g);
      return;
    }
  }

  /* we must change our walking direction */

  /* get new direction */
  d = sn->curdir + random(3);
  if (d >= 4)
    d -= 4;
  o = (unsigned char) random(1);

  for (i=0; i<4; i++)
  {
    sx = DIR2STEPX(d);
    sy = DIR2STEPY(d);
    if (teststep(sn, sx, sy) != 0)
    {
      break;
    }
    if (o == 0)
    {
      d = (d - 1) & 3;
    }
    else
    {
      d = (d + 1) & 3;
    }
  }

  if (i<4)
  {
    /* go into new direction */
    sn->stepx   = sx;
    sn->stepy   = sy;
    sn->curdir  = d;
    step(sn);
  }
  else
  {
    /* We can not go in any direction, so we try to go back. */
    if (--sn->holdcntr == 0)
    {
      sn->holdcntr = MAX_SNAKE_LEN;
      goback(sn);
    }
    else
    {
      if (--sn->seccntr == 0)
      {
        sn->seccntr = sn->speed;
        a = sn->addr[sn->head];
        if ( isBarrier(sn, a + 1) &&
             isBarrier(sn, a - 1) &&
             isBarrier(sn, a + SCREEN_MAXX) &&
             isBarrier(sn, a - SCREEN_MAXX))
        {
          goback(sn);
        }
      }
    }
  }

  /* quit exclusive screen access */
  posSemaSignal(screensem_g);
}



/* Snake task.
 * This function runs in multiple instances, one instance per snake.
 */
void snakeTask(void *arg)
{
  snake_t *sn = (snake_t*) arg;
  UINT_t  sleepticks = HZ / sn->speed;

  for (;;)
  {
    walk(sn);
    posTaskSleep(sleepticks);

    if (stopflag_g != 0)
    {
      nosMemFree(sn);
      posSemaSignal(joinsema_g);
      break;
    }
  }
}



/* Create a new snake.
 * The snake is generated as an independent object
 * in a separate task.
 */
void newSnake(char speed, char length, char excitement)
{
  snake_t       *sn;
  signed   int  p;
  unsigned char d;

  /* allocate memory for snake object */
  sn = (snake_t*) nosMemAlloc(sizeof(snake_t));
  if (sn == NULL)
    return;
  nosMemSet(sn, 0, sizeof(snake_t));

  /* get exclusive access to the screen */
  posSemaGet(screensem_g);

  /* get snake starting point on the screen */
  do
  {
    p = (signed int) random(SCREEN_SIZE);
  }
  while (GETSCREEN(p) != CHAR_SNAKE_NONE);

  /* initialize snake object */
  d = (unsigned char) random(4);
  sn->addr[0]    = p;
  sn->dir[0]     = d;
  sn->head       = 0;
  sn->tail       = 0;
  sn->speed      = speed;
  sn->seccntr    = speed;
  sn->maxlen     = length;
  sn->curdir     = d;
  sn->stepx      = DIR2STEPX(d);
  sn->stepy      = DIR2STEPY(d);
  sn->color      = SNAKECOLOR(snakecounter_g);
  sn->excitement = excitement;

  /* print head of snake */
  PUTCOLOR (p, sn->color);
  PUTSCREEN(p, CHAR_SNAKE_HEAD);

  /* Create snake task. But note:
   * We need two priority levels to get 9 snakes running
   * because only 8 tasks can share the same priority level.
   */
  if (nosTaskCreate(snakeTask, sn, PRIO_SNAKE1, 0, "snake1") == NULL)
    if (nosTaskCreate(snakeTask, sn, PRIO_SNAKE2, 0, "snake2") == NULL)
  {
    PUTSCREEN(p, CHAR_SNAKE_NONE);
    posSemaSignal(screensem_g);
    return;
  }

  /* one more snake */
  snakecounter_g++;

  /* quit exclusive screen access */
  posSemaSignal(screensem_g);

  /* let snake walk a bit */
  posTaskSleep(MS(100));
}


/*-------------------------------------------------------------------------*/


/* Keyboard input task.
 * This task waits for a keypress and sends the key code
 * via message to the main program task. This is necessary
 * to be able to wait with timeout for a keypress.
 */
void keyboardTask(void *arg)
{
  POSTASK_t htask = (POSTASK_t) arg;
  char key, *buf;

  for (;;)
  {
    key = nosKeyGet();
    buf = (char*) posMessageAlloc();
    if (buf != NULL)
    {
      *buf = key;
      posMessageSend(buf, htask);
    }
  }
}



/* Wait for keypress and print CPU usage
 * one time per second as long as no key is pressed.
 */
char getkey(void)
{
  char  *msg;
  JIF_t  j;
  char   c;

  for (;;)
  {
    j = jiffies;

    if (POS_TIMEAFTER(j, nextjiffies_g))
    {
      nextjiffies_g = j + MS(1000);
      printCPUusage();
    }

    msg = (char*) posMessageWait((UINT_t) (nextjiffies_g - j));
    if (msg != NULL)
    {
      c = *msg;
      posMessageFree(msg);
      break;
    }
  }

  return c;  
}


/*-------------------------------------------------------------------------*/


/* Print a black screen with gray frame.
 */
void printFrame(void)
{
  int a = 0;

  SETSCREENCOLOR(0);
  SETTEXTCOLOR(COLOR_FRAME);
  nosPrintChar(147);

  for (a = 0; a < SCREEN_MAXX; a++)
  {
    PUTSCREEN(a, CHAR_FRAME);
    PUTSCREEN(a + (SCREEN_MAXX * (SCREEN_MAXY-1)), CHAR_FRAME);
  }
  for (a = 0; a < (SCREEN_MAXX * SCREEN_MAXY); a += SCREEN_MAXX)
  {
    PUTSCREEN(a, CHAR_FRAME);
    PUTSCREEN(a + (SCREEN_MAXX-1), CHAR_FRAME);
  }

  nosPrint("\x13\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  nosPrint("\x12" POS_STARTUPSTRING "\x92\x13");
}



/* print CPU usage
 */
void printCPUusage(void)
{
  SETTEXTCOLOR(COLOR_FRAME);
  nosPrintf1("\x13\x12 CPU usage:% 3d%%\x92", nosCpuUsage());
}



/* print number of snakes
 */
void printNmbrOfSnakes(void)
{
  SETTEXTCOLOR(COLOR_FRAME);
  nosPrintf1("\x13\x12\x1D\x1D\x1D\x1D\x1D\x1D\x1D\x1D"
             "\x1D\x1D\x1D\x1D\x1D\x1D\x1D\x1D\x1D\x1D"
             "\x1D\x1D\x1D\x1D\x1D\x1D\x1D\x1D\x1D\x1D\x1D"
             "Snakes: %d\x92", snakecounter_g);
}


/*-------------------------------------------------------------------------*/


/* main program
 */
void mainfunc(void *arg)
{
  POSTASK_t htask;
  char  n, i, key;
  char  speed, len, exc;

  (void) arg;

  randsem_g   = posSemaCreate(1);
  screensem_g = posSemaCreate(1);

  if ((randsem_g == NULL) || (screensem_g == NULL))
  {
    nosPrint("Something failed.\n");
    return;
  }

  /* create keyboard input task */
  htask = posTaskGetCurrent();
  nosTaskCreate(keyboardTask, htask, posTaskGetPriority(htask),
                0, "keyboard task");

  nextjiffies_g = jiffies;

  for(;;)
  {
    snakecounter_g = 0;
    stopflag_g = 0;

    printFrame();

    SETTEXTCOLOR(COLOR_TEXT);
    nosPrint("\x13\n\n\x1D Snakes Demo\n\n");

    nosPrint("\x1D Every snake in this demo is an\n");
    nosPrint("\x1D object that runs in a separate task.\n");
    nosPrint("\x1D Thus this prgram demonstrates\n");
    nosPrint("\x1D multitasking on the Commodore 64.\n\n");
    nosPrint("\x1D The RTOS used is pico]OS. Visit\n");
    nosPrint("\x1D http://picoos.sourceforge.net for\n");
    nosPrint("\x1D more details about pico]OS.\n\n");
    nosPrint("\x1D Hint: Start this demo with one snake\n");
    nosPrint("\x1D first and observe the CPU usage.\n");
    nosPrint("\x1D Then press ENTER to add more snakes.\n");
    nosPrint("\x1D If you press any other key, the\n");
    nosPrint("\x1D snakes will be deleted and this\n");
    nosPrint("\x1D screen is shown again.\n");

    nosPrint("\n\n\x1D Enter number of snakes (1-9)? \xAF");

    do
    {
      key = getkey();
    }
    while ((key < '1') || (key > '9'));
    n = key - '0';

    srand((unsigned)jiffies);

    printFrame();

    /* start n snake tasks */
    for (i = n; i > 0; i--)
    {
      len = MIN_SNAKE_LEN;
      if (snakecounter_g < 5)
      {
        speed = 5 + random(3);
        len += random(VAR_SNAKE_LEN +1);
      }
      else
      {
        speed = 4 + random(2);
        len += random(VAR_SNAKE_LEN/2 +1);
      }
      exc = 10 + random(4);
      if (n < 4)
        exc -= 5;

      newSnake(speed, len, exc);
    }

    printNmbrOfSnakes();

    /* wait for key */
    for (;;)
    {
      key = getkey();
      if (key == 13)
      {
        len = MIN_SNAKE_LEN;
        if (snakecounter_g < 4)
        {
          speed = 5 + random(3);
          len += random(VAR_SNAKE_LEN +1);
        }
        else
        {
          speed = 4 + random(2);
          len += random(VAR_SNAKE_LEN/2 +1);
        }
        exc = 10 + random(4);
        newSnake(speed, len, exc);
        printNmbrOfSnakes();
      }
      else
      {
        break;
      }
    }

    /* stop tasks */
    joinsema_g = posSemaCreate(1 - snakecounter_g);
    stopflag_g = 1;
    posSemaGet(joinsema_g);
    posSemaDestroy(joinsema_g);
  }
}
