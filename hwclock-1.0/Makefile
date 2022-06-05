

all: hwclock.64 hwclock.128 hwclock.610

hwclock.64: hwclock.c
	cl65 -t c64 -Osir -o hwclock.64 hwclock.c
hwclock.128: hwclock.c
	cl65 -t c128 -Osir -o hwclock.128 hwclock.c
hwclock.610: hwclock.c
	cl65 -t cbm610 -Osir -o hwclock.610 hwclock.c

clean:
	$(RM) hwclock.64
	$(RM) hwclock.128
	$(RM) hwclock.610
