all: autorun.prg

clean:
	$(RM) autorun.o autorun.prg

autorun.prg: autorun.s
	ca65 autorun.s
	ld65 -C autorun.cfg -o autorun.prg autorun.o

clean:
	$(RM) autorun.prg
	$(RM) autorun.o
