cflags = -c -g -tc64 -Oirs $(inc) -D DEBUG=$(DEBUG) -D NDEBUG
caflags = -tc64 -g -D DEBUG=$(DEBUG) $(inc)
exoflags = sfx basic -n 
#if compiling the final version -p -m should not be used!!!

lib = ../../lib
libsrc = ../../libsrc
inc = -I. -I ./datafiles -I ./include
DEBUG = 0
#this DEBUG flag has nothing to do with NDEBUG,
#which is used by assert.h
#if DEBUG=1, then rasters are shown to see which time
#the routines need and also a debug rasterbar is visible
 
datafiles = datafiles/sunscrdata datafiles/sunxsin datafiles/sunysin \
	datafiles/suncharset datafiles/bergcharset datafiles/bergscrdata \
	datafiles/bergcolordata datafiles/bergspr_old datafiles/clowdsprbank1 \
	datafiles/clowdsprbank2

objects = subs/sun/scrollsun.o\
	subs/sun/fadesun.o\
	subs/clowds/scrollclowds.o\
	subs/clowds/setclowds.o\
	subs/clowds/copyclowds2.o\
	subs/mountains/InitMountains.o \
	subs/mountains/setmountainspr.o \
	subs/mountains/scrllmountains.o \
	subs/mountains/textscroll.o \
	subs/misc/InitParScreen.o \
	subs/misc/data.o \
	subs/misc/spdcgen.o \
	subs/misc/barcode.o \
	subs/misc/constants.o \
	subs/misc/colorfade.o \
	subs/spr/setspry.o \
	subs/spr/sprinit.o \
	subs/irq/upperirq.o\
	subs/irq/boffirq.o \
	subs/irq/bergirq.o \
	subs/irq/irqinit.o \
	mainloop.o
	

#.PHONY : all
all: paralax.run paralax.d64

data.o: data.s $(datafiles)
	ca65 $(caflags) $<
	ar65 a paralax.lib $@
	
paralax.run: parmain.c paralax.cfg $(objects)
	cl65 -C paralax.cfg -Oirs -g --mapfile paralax.map -Ln labels $(inc) -D DEBUG=$(DEBUG) $< $(objects) -o $@

paralax.exo: paralax.run
	exomizer $(exoflags) -o paralax.exo paralax.run 
	
paralax.d64: paralax.exo
	c1541 -format paralax,07 d64 $@ -write $<

randfade/randfade.o: randfade/randfade.c
	cd randfade
	make -f rand.mak
	cd ..

%.o: %.c
	cl65 $(cflags) $<

%.o: %.s
	ca65 $(caflags) $<
#subs/sun/%.o: subs/sun/%.s
#	ca65 $(caflags) $<
	
#.PHONY: clean
clean :
	$(RM) *.o
	$(RM) $(objects)
	$(RM) *.lib
	$(RM) *.lst
	$(RM) labels
	$(RM) paralax.run
	$(RM) paralax.exo
	$(RM) paralax.d64
	$(RM) paralax.map
	
