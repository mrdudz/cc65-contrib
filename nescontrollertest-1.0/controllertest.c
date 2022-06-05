/*NES Controller Input Test
  Version 1.0
  Paul Hackmann
  12/30/05
  This code reads the button states from Joypad 1 and prints
  which button has been pressed.
*/

#include <conio.h>

#define reg_joypad1 *(char*)0x4016
#define reg_joypad2 *(char*)0x4017

void main(void)
{
	unsigned char a, olda;
	unsigned char b, oldb;
	unsigned char select, oldselect;
	unsigned char start, oldstart;
	unsigned char up, oldup;
	unsigned char down, olddown;
	unsigned char left, oldleft;
	unsigned char right, oldright;
	unsigned char i;

	//start by clearing all of the button state variables
	a = 0;
	olda = 0;
	b = 0;
	oldb = 0;
	select = 0;
	oldselect = 0;
	start = 0;
	oldstart = 0;
	up = 0;
	oldup = 0;
	down = 0;
	olddown = 0;
	left = 0;
	oldleft = 0;
	right = 0;
	oldright = 0;

	clrscr();  //blank the screen
	cprintf("Starting Controller Test\n\r");	//lets us know the program is running

	//strobe the joypad to reset it
	reg_joypad1 = 1;
	reg_joypad1 = 0;
	//check the signature, refer to the nes tech docs for more info on signatures
	for(i=1;i<21;i++)
	{
		a = reg_joypad1;
	}
	//if we get a "1" at this location, it indicates a joypad is connected to port 1. 
	//(this may be less than fully correct, but seems to work with FCE Ultra)
	if(a == 0x41)
	{
		cprintf("Joypad 1 Found\n\r");
	}
	a = 0;

	//strobe the joypad to reset it
	reg_joypad1 = 1;
	reg_joypad1 = 0;

	while(1)	//loop forever
	{
		//read joypad1 A button from $4016
		a = reg_joypad1;
		if((a == 0x41) && (olda == 0x40))
		{
			cprintf("A");
		}
		//read joypad1 B button from $4016
		b = reg_joypad1;
		if((b == 0x41) && (oldb == 0x40))
		{
			cprintf("B");
		}
		//read joypad1 Select button from $4016
		select = reg_joypad1;
		if((select == 0x41) && (oldselect == 0x40))
		{
			cprintf("Select");
		}
		//read joypad1 Start button from $4016
		start = reg_joypad1;
		if((start == 0x41) && (oldstart == 0x40))
		{
			cprintf("Start");
		}
		//read joypad1 Up button from $4016
		up = reg_joypad1;
		if((up == 0x41) && (oldup == 0x40))
		{
			cprintf("Up");
		}
		//read joypad1 Down button from $4016
		down = reg_joypad1;
		if((down == 0x41) && (olddown == 0x40))
		{
			cprintf("Down");
		}
		//read joypad1 Left button from $4016
		left = reg_joypad1;
		if((left == 0x41) && (oldleft == 0x40))
		{
			cprintf("Left");
		}
		//read joypad1 Right button from $4016
		right = reg_joypad1;
		if((right == 0x41) && (oldright == 0x40))
		{
			cprintf("Right");
		}

		//update the previous button state holders
		olda = a;
		oldb = b;
		oldselect = select;
		oldstart = start;
		oldup = up;
		olddown = down;
		oldleft = left;
		oldright = right;

		//strobe the joypad again to reset it
		reg_joypad1 = 1;
		reg_joypad1 = 0;
	}
}