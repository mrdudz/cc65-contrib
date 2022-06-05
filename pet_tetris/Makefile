

all: tetris 8ktetris dactetris

tetris: tetris.c platform.c tetris_funcs.asm
# Make standard PET Tetris (16- and 32k machines)
	cc65 -t pet -O -T tetris.c
	ca65 -t pet tetris.s -o tetris.o
	cc65 -t pet -O -T platform.c
	ca65 -t pet platform.s -o platform.o
	ca65 -t pet tetris_funcs.asm -o tetris_funcs.o
	cl65 -t pet tetris.o platform.o tetris_funcs.o -m tetris.map -o tetris
	
# Make 8K (cut-down) version of PET Tetris (eg for 2001)
8ktetris: tetris.c platform.c tetris_funcs.asm
	cc65 -t pet -Osir -T -DTETRIS_8K tetris.c
	ca65 -t pet tetris.s -o tetris.o
	cc65 -t pet -Osir -T -DTETRIS_8K platform.c
	ca65 -t pet platform.s -o platform.o
	ca65 -t pet -DTETRIS_8K tetris_funcs.asm -o tetris_funcs.o
	cl65 -t pet tetris.o platform.o tetris_funcs.o -m 8ktetris.map -o 8ktetris

# Make DACTETRIS
dactetris: tetris.c platform.c tetris_funcs.asm
	cc65 -t pet -Osir -T -DDAC_TETRIS tetris.c
	ca65 -t pet tetris.s -o tetris.o
	cc65 -t pet -Osir -T -DDAC_TETRIS platform.c
	ca65 -t pet platform.s -o platform.o
	ca65 -t pet -DDAC_TETRIS tetris_funcs.asm -o tetris_funcs.o
	cl65 -t pet tetris.o platform.o tetris_funcs.o -m dactetris.map -o dactetris

clean:
	$(RM) tetris
	$(RM) 8ktetris
	$(RM) dactetris
	$(RM) tetris.map
	$(RM) 8ktetris.map
	$(RM) dactetris.map
	$(RM) tetris.s
	$(RM) platform.s
	$(RM) tetris.o
	$(RM) platform.o
	$(RM) tetris_funcs.o
	$(RM) *.o

