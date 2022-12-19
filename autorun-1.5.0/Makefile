
all: autorun.prg

autorun.prg: autorun.s
	ca65 autorun.s
	ld65 -C autorun.cfg -o autorun.prg autorun.o

hello.prg: hello.c
	cl65 -o hello.prg hello.c

autorun.d64: autorun.prg hello.prg
	c1541 -format "autorun,00" d64 autorun.d64 \
		-write autorun.prg autorun \
		-write hello.prg hello

disk: autorun.d64

clean:
	$(RM) autorun.prg
	$(RM) autorun.o
	$(RM) hello.prg
	$(RM) autorun.d64
