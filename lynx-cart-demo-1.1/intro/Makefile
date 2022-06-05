#
# makefile for the www.cc65.org compiler version of abccreator
#

CC=cc65 -t lynx -Oirs
RM=rm -f
CP=cp
SPRPCK=sprpck -p1 -t6
ECHO=echo
CODE_SEGMENT=INTRO_CODE
DATA_SEGMENT=INTRO_DATA
RODATA_SEGMENT=INTRO_RODATA
BSS_SEGMENT=INTRO_BSS

.SUFFIXES: .o .c .s

all:	intro.s

intro.s: intro.c Makefile
	$(CC) --code-name $(CODE_SEGMENT) \
	--rodata-name $(RODATA_SEGMENT) \
	--bss-name $(BSS_SEGMENT) \
	--data-name $(DATA_SEGMENT) $*.c

clean :
	$(RM) intro.s
	$(RM) *.o

