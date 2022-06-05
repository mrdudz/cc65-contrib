
TARGET?=c64

PROGRAM=3dmaze

FILES=3dmaze.c

TARGETPATH=$(shell cl65 --print-target-path)/$(TARGET)

$(PROGRAM)-$(TARGET): $(FILES)
	cl65 -t $(TARGET) -Osir -o $(PROGRAM)-$(TARGET) $(FILES)

disk: $(PROGRAM)-$(TARGET)
	c1541 -format "3dmaze,00" d64 $(PROGRAM)-$(TARGET).d64 \
		-write $(PROGRAM)-$(TARGET) \
		-write $(TARGETPATH)/drv/tgi/c64-hi.tgi
	
clean:
	$(RM) $(PROGRAM)-$(TARGET)
