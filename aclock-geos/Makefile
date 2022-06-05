all: aclock-geos-apple aclock-geos-cbm


aclock-geos-apple: aclock-geos.c
	cl65 -t geos-apple -o aclock-geos-apple aclock-geos.c
aclock-geos-cbm: aclock-geos.c
	cl65 -t geos-cbm -o aclock-geos-cbm aclock-geos.c

clean:
	$(RM) aclock-geos-apple
	$(RM) aclock-geos-cbm
