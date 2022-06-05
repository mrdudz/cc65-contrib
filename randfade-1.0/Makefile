
TARGET?=c64

PROGRAM=randfade.prg

FILES=randfade.c

$(PROGRAM): $(FILES)
	cl65 -t $(TARGET) -Osir -o $(PROGRAM) $(FILES)

clean:
	$(RM) $(PROGRAM)
