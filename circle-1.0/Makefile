
TARGET?=c64

PROGRAM=circle

FILES=circle.c

$(PROGRAM): $(FILES)
	cl65 -t $(TARGET) -Osir -o $(PROGRAM) $(FILES)

clean:
	$(RM) $(PROGRAM)
