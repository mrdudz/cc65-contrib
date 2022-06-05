TARGET?=c64

all: huffman-$(TARGET).prg decoder-$(TARGET).prg

decoder-$(TARGET).prg: decoder.c
	cl65 -Osir -t $(TARGET) -o decoder-$(TARGET).prg decoder.c
huffman-$(TARGET).prg: huffman.c
	cl65 -Osir -t $(TARGET) -o huffman-$(TARGET).prg huffman.c

clean:
	$(RM) decoder-$(TARGET).prg
	$(RM) decoder-c64.prg
	$(RM) decoder-c128.prg
	$(RM) huffman-$(TARGET).prg
	$(RM) huffman-c64.prg
	$(RM) huffman-c128.prg
