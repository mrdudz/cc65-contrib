
TARGET?=c64

TARGETPATH=$(shell cl65 --print-target-path)/$(TARGET)

all: tgi-preview.prg

tgi-preview.prg: tgi-preview.c
	cl65 -t $(TARGET) -Osir -o tgi-preview.prg tgi-preview.c

disk: tgi-preview.prg
	c1541 -format "tgi-preview,00" d64 tgi-preview.d64 \
		-write tgi-preview.prg tgi-preview \
		-write goth.tch \
		-write litt.tch \
		-write scri.tch \
		-write $(TARGETPATH)/drv/tgi/c64-hi.tgi

clean:
	$(RM) tgi-preview.prg
	$(RM) tgi-preview.d64
