
TARGET?=c64

PROGRAM=darth-vader

FILES=darth-vader.c

$(PROGRAM): $(FILES)
	cl65 -t $(TARGET) -Osir -o $(PROGRAM) $(FILES)

clean:
	$(RM) $(PROGRAM)
