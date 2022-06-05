// A silly application that allows you to draw pixels around the screen.
// Change colors with Fire 2 (in case your driver supports a second fire
// button).
// Keeping Fire pressed works as an eraser.

#include <lynx.h>
#include <6502.h>
#include <conio.h>
#include <joystick.h>
#include <tgi.h>
#include "../resident/resident.h"
#include "../resident/abcmusic.h"
#define tgi_flip() tgi_ioctl(1, 0)
#define tgi_setbgcolor(val) tgi_ioctl(2, (unsigned)(val))

static char drawPending;

#define DELAY 200

static void
usleep(int n)
{
    int i;
    for (i = 0; i < n; i++)
	abc_update();
}

static int sintbl[9] = {
	0,
	3196,  // 11.25
	6270,  // 22.5
	9102,  // 33.75
	11585, // 45
	13623, // 56.25
	15137, // 67.5
	16069, // 78.75
	16384  // 90
};

static int sin(char d)
{
    char neg;
    d = d & 31;
    neg = d > 16;
    d = d & 15;
    if (d > 8)
        d = 16 - d;
    if (neg)
	return -sintbl[d];
    else
	return sintbl[d];
}

static void circle(int x0, int y0, unsigned char r)
{
	char i;
	int x1, y1, x2, y2;

	x1 = ((long)sin(0) * r + 8192) / 16384 + x0;
	y1 = ((long)sin(8) * r + 8192) / 16384 + y0;
	for (i = 1; i <= 32; i++) {
		x2 = ((long)sin(i) * r + 8192) / 16384 + x0;
		y2 = ((long)sin(i+8) * r + 8192) / 16384 + y0;
		tgi_line(x1, y1, x2, y2);
		x1 = x2;
		y1 = y2;
	}
}
#define tgi_circle circle

extern unsigned char drawpage;

int sketch(void)
{
  int x, y, w, h, frame, r;
  unsigned char color, nrofcolors, background, framecolor;
  char *stitle = "Lynx-Sketch";

  framecolor = COLOR_RED;

  background = COLOR_GREY;

  nrofcolors = tgi_getcolorcount();
  w = tgi_getxres();
  h = tgi_getyres();

  // Single display buffer
  tgi_setviewpage(drawpage);
  tgi_setdrawpage(drawpage);

  // Set our default palette to the hardware
  tgi_setpalette(tgi_getdefpalette());

  // Draw an Etch-A-Scetch style background
  tgi_setcolor(framecolor);
  tgi_bar(0, 0, w-1, h-1);
  tgi_setcolor(background);
  frame = h / 8;
  tgi_bar(frame, frame, w-frame-1, h-frame-1);

  // Set our pen to the center
  x = w / 2;
  y = h / 2;
  color = COLOR_BLACK+1;

  tgi_setcolor(background);
  tgi_setbgcolor(0);
  tgi_outtextxy(frame, frame-10, stitle);

  // Start our application
  drawPending = 0;
  while (1) {
    unsigned char eraser;
    unsigned char joy;
    unsigned char oldcolor;
    joy = joy_read(JOY_1);
    if (JOY_BTN_UP(joy)) {
      y--;
      if (y < frame + 1)
	y = frame + 1;
      drawPending = 1;
    }
    if (JOY_BTN_DOWN(joy)) {
      y++;
      if (y > h - frame - 2)
	y = h - frame - 2;
      drawPending = 1;
    }
    if (JOY_BTN_LEFT(joy)) {
      x--;
      if (x < frame + 1)
	x = frame + 1;
      drawPending = 1;
    }
    if (JOY_BTN_RIGHT(joy)) {
      x++;
      if (x > w - frame - 2)
	x = w - frame - 2;
      drawPending = 1;
    }
    if (JOY_BTN_FIRE(joy)) {
      eraser = 1;
      drawPending = 1;
    } else {
      eraser = 0;
    }
    if (JOY_BTN_FIRE2(joy)) {
      drawPending = 1;
      color++;
      if (color > nrofcolors)
          color = 1;
    }
    if (drawPending) {
      if (eraser) {
        tgi_setcolor(background);
        tgi_bar(x-1, y-1, x+1, y+1);
	r--;
      } else {
        tgi_setcolor(color);
        tgi_setpixel(x, y);
      }
      drawPending = 0;
    }
    // Blink cursor
    oldcolor = tgi_getpixel(x, y);
    tgi_setcolor(oldcolor ^ (nrofcolors - 1));
    tgi_setpixel(x, y);
    usleep(DELAY);
    tgi_setcolor(oldcolor);
    tgi_setpixel(x, y);
    usleep(DELAY);
    if (kbhit()) {
        switch (cgetc()) {
        case 'F':
            tgi_flip();
            break;
        case 'R':
            silence();
            drawpage = 1 - drawpage;
            return RESTART_LYNX;
        case '1':
            silence();
            drawpage = 1 - drawpage;
            return NEXT_APPLICATION;
        case '2':
            silence();
            drawpage = 1 - drawpage;
            return PREV_APPLICATION;
        }
    }
  }
}
