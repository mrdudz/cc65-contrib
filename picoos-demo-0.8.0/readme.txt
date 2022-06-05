
          Snakes Multitasking Demo  for Commodore 64
         --------------------------------------------

This  program  demonstrates  multitasking  on  the  Commodore 64.
The program  is based  on the  pico]OS  realtime operating system
(http://picoos.sourceforge.net).

The  program uses  the pico-  and  the nano- layer  of  the RTOS,
both  configuration files poscfg.h and  noscfg.h are shipped with
this distribution. Note  that  the  configuration  files are  for
pico]OS  version 0.8.0.  Newer  versions  of pico]OS  should work
fine  also, but you may need  to update the config  files (simply
add all missing  feature defines to the files and set them to 0).

You need this directory structure to be able to compile the demo:

repository/contrib/demos/6502-c64/snakes   - this demo
repository/picoos/[...]                    - pico]OS RTOS

If you have an  other directory structure, you need to change the
makefile.  Set  the  variable  "RELROOT"  to  your  picoos   root
directory, and set "DIR_CONFIG" accordingly.

To build the demo, simply  execute the GNU makefile stored in the
snakes  directory ("all" is  the  default target). The  generated
executable can be found here:  picoos/out/6502/rel/snakes.c64
You may use the VICE emulator to execute the program.

Download pico]OS RTOS at  http://sourceforge.net/projects/picoos/
Download CC65 compiler at http://www.cc65.org/
Download VICE emulator at http://www.viceteam.org/
