AVR-Port
--------

This is a pre-release of the AVR port. Maybe not all bugs are found yet. 
The code has been only tested on an ATMEGA323 compiled with avr-gcc 3.3.1 and 
avr-libc 0.99.90.20030829.

TODO:

- test the pico]os-features:
  Create programs for testing the features of the os (semaphores, mutex, 
  events, messages, ect.).

- malloc() and free():
  It shold be found a way to use malloc() and free() from avr-libc whithout 
  memory damage cause by a thread. First workaround can be a lock around the call of
  these functions.
  
- extend for other AVR-architectures:
  It have to be found a way to support the different designs of the timer in
  the different devices. Maybe by test the device by the same define, that is
  used by avr-libc.

- documentation and examples:
  Create pattern-like example programms to demonstrate the different features.
  It should provide an easy start for beginners by using pico]os and RTOS in general.
  
  