
TARGET?=c64

PROGRAM=fixedpoint-example

FILES=fixedpoint.c fixedpoint-example.c

$(PROGRAM): $(FILES)
	cl65 -t $(TARGET) -Osir -o $(PROGRAM) $(FILES)

clean:
	$(RM) $(PROGRAM)
