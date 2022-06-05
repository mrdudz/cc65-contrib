DEBUG = 0

cflags = -c -g -tc64 -Oirs $(inc) -D DEBUG=$(DEBUG)
clflags = -c -g -tc64 -Oirs $(inc) -D DEBUG=$(DEBUG)
caflags = -tc64 -g -D DEBUG=$(DEBUG) $(inc)
exoflags = -s 0x080d -n -m 2048 -p 2
lib = ../../lib
libsrc = ../../libsrc
inc = -I. -I./datafiles -I./subs/headers


datafiles = \
	datafiles/logo.bmp \
	datafiles/logo.scr \
	datafiles/logo.col \
	datafiles/Intro.mus \
	datafiles/1x2charset_hires.prg \
	mountains/paralax.exo

OBJS = \
	data.o \
	subs/showlogo/showlogo.o \
	subs/fld/fld.o \
	subs/waitretrace/waitretrace.o \
	subs/createparabel/creatparabel.o \
	subs/irq/irq.o \
	subs/irq/rasterirq.o \
	subs/fld/logofader.o \
	subs/fontdisp/fontdisp.o \
	subs/fontdisp/writetext.o \
	subs/textfader/makefadetab.o \
	subs/textfader/fadecodesnibble.o \
	subs/textfader/createfadecode.o \
	subs/textfader/textfader.o \
	subs/randfade/randfade.o \
	subs/kickborder/kickborder.o \
	subs/showlogo/copylogocolors.o \
	subs/irq/setsplitirq.o \
	subs/rmemcpy/rmemcpy.o

objects = $(OBJS) \
	subs/misc/delay_loop.c \
	subs/misc/getspacestat.s \
	subs/misc/zpsecure.s \
	subs/misc/musicfader.c

all: intro.exo intro.d64

mountains/paralax.exo:
	make -C mountains paralax.exo

data.o: data.s $(datafiles)
	cl65 $(cflags) $<

intro.run: intro.c intro.cfg $(objects)
	cl65 -m intro.map -l intro.lst -C intro.cfg -Oirs -g -Ln labels $(inc) -D DEBUG=$(DEBUG) $< $(objects) -o $@

intro.exo: intro.run
	exomizer3 sfx 0x80d -o intro.exo intro.run

intro.d64: intro.exo
	c1541 -format "paralax,07" d64 $@ -write $<

%.o: %.c 
	cl65 $(clflags) $<

%.o: %.s 
	ca65 $(caflags) $<

.PHONY: clean
clean :
	$(RM) *.o
	$(RM) $(OBJS)
	$(RM) intro.lst
	$(RM) intro.map
	$(RM) intro.exo
	$(RM) intro.run
	$(RM) intro.d64
	make -C mountains clean

