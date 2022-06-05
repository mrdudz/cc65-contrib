TARGET?=c64

all: loadkoala

loadkoala: loadkoala.c
	cl65 -Osir -t $(TARGET) -o loadkoala loadkoala.c

disk: loadkoala.d64

loadkoala.d64: loadkoala cyberwing.koa
	c1541 -format "loadkoala,00" d64 loadkoala.d64 \
		-write loadkoala \
		-write cyberwing.koa
	
clean:
	$(RM) loadkoala

