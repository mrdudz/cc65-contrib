
TARGET?=c64

all: isoline

isoline: isoline.c
	cl65 -t $(TARGET) -Osir -o isoline isoline.c
	
clean:
	$(RM) isoline
