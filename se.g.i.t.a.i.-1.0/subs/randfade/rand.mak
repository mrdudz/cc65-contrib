cflags = -c -g -tc64 -Oirs -D DEBUG=$(DEBUG) -l

all: randfade.c randfade.h
	cl65 $(cflags) $<
	