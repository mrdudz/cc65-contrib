
TARGET?=c64

SQRTLIBSRC= sqr8.s sqrt8.s sqrt16.s

all: sqrt colcyc

sqrt:	sqrt.c $(SQRTLIBSRC)
	cl65 -Osir -t $(TARGET) -o sqrt sqrt.c $(SQRTLIBSRC)
	
colcyc:	colcyc.s 16bit.mac $(SQRTLIBSRC)
#	cl65 -t $(TARGET) -C$(TARGET)-asm.cfg -o colcyc colcyc.s $(SQRTLIBSRC) $(TARGET).lib
	cl65 -Osir -t $(TARGET) -l colcyc.lbl -o colcyc colcyc.s $(SQRTLIBSRC)

clean:
	$(RM) sqrt
	$(RM) sqrt.lbl
	$(RM) sqrt.map
	$(RM) colcyc
	$(RM) colcyc.lbl
	$(RM) colcyc.map
