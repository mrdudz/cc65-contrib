
TARGET?=c64

PROGRAM=testspdc

clflags = -g -Ln labels -Oirs -t $(TARGET) -I .

objects = testspdc.o testspdcasm.o spdc.o

$(PROGRAM): $(objects)
	cl65 $(clflags) $(objects) -o $(PROGRAM)
	
%.o: %.c
	cl65 -c $(clflags) $< -o $@
	
%.o: %.s
	ca65 $(caflags) $< -o $@
	
clean:
	$(RM) $(PROGRAM)
	$(RM) *.o	
	$(RM) labels
