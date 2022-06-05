all: matrix.prg matrix128.prg

matrix.prg: matrix.c
	cl65 -Osir -o $@ $?

matrix128.prg: matrix2.c
	cl65 -t c128 -Osir -o $@ $?

clean:
	$(RM) *.o
	$(RM) matrix.prg
	$(RM) matrix128.prg
