
TARGET?=nes

PROGRAM=controllertest.nes

FILES=controllertest.c

$(PROGRAM): $(FILES)
	cl65 -t $(TARGET) -Osir -o $(PROGRAM) $(FILES)

clean:
	$(RM) $(PROGRAM)
