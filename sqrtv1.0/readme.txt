This archive contains two procedures for calculating squareroots
of 8 and 16 bit numbers. 
The result is rounded, so that the precision is +-0.5 !


These are named sqrt8 and sqrt16.

They were written with CC65 version 2.10.0 and you should
use this version, because i made extensively use of : labels.

There is an example program, colcyc.s, which makes use of sqrt16.

There is another routine included into the archive, which calculates
squares iterately. It simply starts with one square number and then adds
the right odd number to get to the next square.

There is also a test program sqrt.c, which prints out different numbers and
corresponding square roots and asserts, that the result is correct e.g. lies
in the precision range.

There are no changes planned, so version is 1.0.
But who knows, if some optimizations can be done.

