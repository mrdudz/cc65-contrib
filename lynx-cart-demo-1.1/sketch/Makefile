#
# makefile for the www.cc65.org compiler version of abccreator
#

CC=cc65 -t lynx -Oirs
RM=rm -f
CP=cp
SPRPCK=sprpck -p1 -t6
ECHO=echo
CODE_SEGMENT=SKETCH_CODE
DATA_SEGMENT=SKETCH_DATA
RODATA_SEGMENT=SKETCH_RODATA
BSS_SEGMENT=SKETCH_BSS

.SUFFIXES: .o .c .s

all:	sketch.s

sketch.s: sketch.c
	$(CC) --code-name $(CODE_SEGMENT) \
	--rodata-name $(RODATA_SEGMENT) \
	--bss-name $(BSS_SEGMENT) \
	--data-name $(DATA_SEGMENT) $*.c

clean :
	$(RM) sketch.s
	$(RM) *.o
