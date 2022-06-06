
TARGET?=atari
#CONFIG?=-C atari.cfg

CFLAGS=-Osi

all: calomin.xex

clm1.asm: main.c
	@echo "Creating main assembler source: main.c to clm1.asm ..."
	cc65 -t $(TARGET) $(CONFIG) $(CFLAGS) -o clm1.asm main.c

clm1.o: clm1.asm
	@echo "Assembling main source: clm1.asm to clm1.o ..."
	ca65 -t $(TARGET) -o clm1.o clm1.asm

clm_sup.o: rmt_sup.asm
	@echo "Assembling the supplementary routines: rmt_sup.asm to clm_sup.o ..."
	ca65 -t $(TARGET) -o clm_sup.o rmt_sup.asm

clm1.xex: clm_sup.o clm1.o
	@echo "Linking: clm1.o and clm_sup.o to clm1.xex"
	cl65 -t $(TARGET) $(CONFIG) $(CFLAGS) -o clm1.xex clm_sup.o clm1.o 

calomin.xex: clm1.xex
	@echo "Linking data: Extending clm1.xex resulting into calomin.xex"
	java -cp . Minilinker clm1.xex calomin.xex\
			34816 data/clmfont.fnt\
			24576 data/dl_cave.dat\
			41000 levels/levels.dat\
			0 data/rmtplayer.obx \
			0 data/music.rmt

test: calomin.xex
	atari800 calomin.xex
	
clean:
	@$(RM) calomin.xex
	@$(RM) clm1.xex
	@$(RM) clm1.asm
	@$(RM) clm_sup.o clm1.o
