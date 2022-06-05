CC=cl65
AS=ca65
LD=ld65
TARGET=c64
CFLAGS=-Oirs -t $(TARGET)
AFLAGS=


%.o: %.c
	$(CC) -c $(CFLAGS) $<

%.o: %.s
	$(AS) $(AFLAGS) $<


OBJS= \
	menutest.o \
	menu.o \
	screen.o


all: menutest.prg


menutest.prg: $(OBJS)
	$(CC) -o menutest.prg $(CFLAGS) $^

clean:
	$(RM) $(OBJS) menutest.prg

distclean: clean
	$(RM) *~
