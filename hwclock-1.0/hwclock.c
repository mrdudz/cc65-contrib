/*****************************************************************************/
/*                                                                           */
/*                                 hwclock.c                                 */
/*                                                                           */
/*                Hardware Clock of C64/C128 and CBM510/CBM610               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) Copyright 2009, Stefan Haubenthal <polluks@sdf.lonestar.org>          */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <cbm.h>

#if defined(__CBM510__) || defined(__CBM610__)
#  define outb(addr,val)	pokebsys ((unsigned)(addr), val)
#  define outw(addr,val)	pokewsys ((unsigned)(addr), val)
#  define inb(addr)		peekbsys ((unsigned)(addr))
#  define inw(addr)		peekwsys ((unsigned)(addr))
#else
#  define outb(addr,val)	(*(unsigned char*) (addr) = (val))
#  define outw(addr,val)	(*(unsigned*) (addr) = (val))
#  define inb(addr)		(*(unsigned char*) (addr))
#  define inw(addr)		(*(unsigned*) (addr))
#endif

#ifndef CIA1
#define CIA1 CIA
#endif

unsigned char atobcd(unsigned char x)
{
return x/10<<4 | x%10;
}

unsigned char addbcd(unsigned char x)
{
return x+(x==8 || x==9 ? 0x18 : 0x12);
}

unsigned char hour, min, sec;

int main (int argc, char *argv[])
{
#if !defined(__CBM610__)
if (argc==2)
	if (sscanf(argv[1], "%d:%02d:%02d", &hour, &min, &sec)==3)
#else
printf("hh:mm:ss? ");
if (scanf("%d:%02d:%02d", &hour, &min, &sec)==3)
#endif
		{
#if !defined(__CBM510__)
		if (get_tv()==TV_PAL)
			outb(&CIA1.cra, inb(&CIA1.cra) | 0x80);
#endif
		outb(&CIA1.tod_hour, hour>11 ? atobcd(hour-12) | 0x80 : atobcd(hour));
		outb(&CIA1.tod_min, atobcd(min));
		outb(&CIA1.tod_sec, atobcd(sec));
		outb(&CIA1.tod_10, 0);
		}
#if !defined(__CBM610__)
	else
		printf("Usage: %s [hh:mm:ss]\n", argv[0]);
#else
printf("\n");
#endif
printf("%x:%02x:%02x.%d\n", inb(&CIA1.tod_hour)&0x80 ? addbcd(inb(&CIA1.tod_hour)&~0x80) :
inb(&CIA1.tod_hour), inb(&CIA1.tod_min), inb(&CIA1.tod_sec), inb(&CIA1.tod_10));
}
