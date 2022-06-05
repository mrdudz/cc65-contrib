#
# makefile to create a complete Atari Lynx cart using the
# the www.cc65.org compiler
#
# Currently the only user applications are "intro" for the startup
# and "sketch" for a drawing application.
# If you want more loadable modules you need to add them just like
# the "sketch" is created.
# Note: you need to add them to this Makefile, to the directory.s
# and you have to create a new directory for the new module.

CC65_ASMINC=$(CC65_INC)/../asminc
AS=ca65 -t lynx
CC=cc65 -t lynx -Oirs
CL=cl65 -t lynx -Oirs
RM=rm -f
CP=cp
SPRPCK=sprpck

CFLAGS=
BMPFLAGS= -p1 -t6

.SUFFIXES: .s .o .c .bmp .spr

.bmp.spr:
	$(SPRPCK) $(BMPFLAGS) $<

%.o: %.s
	$(AS) -I "$(CC65_ASMINC)" -l -o $@ $(AFLAGS) $<

all:	cart.lnx

cart.lnx : \
	lnxhdr.o \
	encrypt1024.o \
	directory.o \
	title.o \
	../resident/resident.o \
	../resident/lynx-stdjoy.o \
	../resident/lynx-160-102-16.o \
	../resident/fileexec.o ../resident/fileload.o \
	../resident/filedir.o ../resident/filehelp.o \
	../resident/abcmusic.o \
	../resident/vbl.o \
	../resident/irq.o \
	../resident/fadein.o \
	../resident/fadeout.o \
	../intro/intro.o \
	../sketch/sketch.o
	$(MAKE) -C ../resident
	$(MAKE) -C ../intro
	$(MAKE) -C ../sketch
	$(CL) -o cart.lnx -m cart.map -C lynxcart.cfg $^ lynx.lib 

huu.o : \	
	lnxhdr.o encrypt1024.o  directory.o title.o \
	../resident/resident.o \
	../resident/lynx-stdjoy.o \
	../resident/lynx-160-102-16.o \
	../resident/fileexec.o ../resident/fileload.o \
	../resident/filedir.o ../resident/filehelp.o \
	../resident/abcmusic.o \
	../resident/vbl.o \
	../resident/irq.o \
	../resident/fadein.o \
	../resident/fadeout.o \
	../intro/intro.o \
	../sketch/sketch.o \
	lynx.lib

clean :
	$(RM) *.o
	$(RM) *.pal
	$(RM) *.spr
	$(RM) *.LYX
	$(RM) *.lyx
	$(RM) *.lnx
	$(RM) *.map
	$(RM) null
