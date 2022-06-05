CP=cp
RM=rm -f

all:
	$(MAKE) -C resident
	$(MAKE) -C intro
	$(MAKE) -C sketch
	$(MAKE) -C cart
	$(CP) cart/cart.lnx .

clean:
	$(MAKE) -C resident clean
	$(MAKE) -C intro clean
	$(MAKE) -C sketch clean
	$(MAKE) -C cart clean
	$(RM) cart.lnx
