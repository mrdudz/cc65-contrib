
TARGET?=c64

CFLAGS = -Oirs -t $(TARGET)

CP=cp

all:	entpack testamica testzoom

#dizzyentp:	dizzyentp.c
#	cl65 $(CFLAGS) -c dizzyentp.c -o dizzyentp
entpack:	entpack.c 

	cl65 $(CFLAGS) -c entpack.c -o entpack.o
        
testamica:	entpack.o testamica.c	

	cl65 $(CFLAGS) entpack.o testamica.c -o testamica

testzoom:	entpack.o testzoom.c

	cl65 $(CFLAGS) entpack.o testzoom.c -o testzoom

#testentp2:	dizzyentp.o testamica.c
#	cl65 $(CFLAGS) dizzyentp.o testentpack.c -o testentp2

clean:	

	$(RM) entpack.o testamica testzoom testentp2
	$(RM) *.map *.lst
	$(RM) testentp.d64

disk:	testamica testzoom

	$(RM) testentp.d64
#	copy template.d64 testentp.d64
#	copy entpack.o ../lib
	$(CP) template.d64 testentp.d64
	c1541 -attach testentp.d64 -write testamica -write testzoom
