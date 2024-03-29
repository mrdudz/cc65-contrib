Lynx Cart Demo 1.1 for the Atari Lynx by Karri Kaksonen, 2005

The Makefiles have changed slightly from version 1.0.
The default installation path for the cc65 tools is now
C:\Program Files\cc65

The space character in the path required some special care.
Also the cl65 does not handle *.s files directly so I added
a separate rule for making the object files.

The cc65 compiler can be tweaked to produce a complete Lynx cart image
with the extra Handy emulator headers. This project shows you how to
do it.

In order to run this on a Windows platform you need to get the unix
commands "rm", "cp" and "make". They can easily be found on the net.
I have tested a package called UnxUtils.zip. It seems to be available
from unxutils.sourceforge.net.

On my linux platform the make runs without any extras.

To compile this you need to do:

cd lynx-cart-demo
make

After this you should get an application called cart.lnx that can be
run in Handy or burned to a ROM.

A small description of what goes into this project.

The first thing is the cart header. It has a fixed 64 byte header for the
Handy emulator (cart/lnxhdr.s). Here you specify the name of the game and
the Author of the cart.

Then comes the encrypted loader (cart/encrypt1024.s). It is a 410 byte
Atari-signed loader that contains the code to load in the cart in a real
Lynx.

The next item is the cart directory (cart/directory.s). It is a file system
that can be accessed by the Lynx at run time. This demo cart has only
four files on the cart.

File 0 is the title sprite. It is just a single black pixel with a black
palette. I rather do my title images in a separate intro module than try
to go through all the trouble with a special-format title sprite.

File 1 is the main program that stays in memory all the time. The code
that goes here is defined in the (resident) directory. It is also defined
as the RAM-segment in the config file (cart/lynxcart.cfg). Every time I
want to load new code from the cart I return to the main program for doing
it. In this way I can be sure that I don't load code over myself accidentally.

File 2 is the intro/startup segment. When the machine starts running I load
this segment as soon as possible and jump to the start_irqs routine to get
drivers and stuff initialized. Then I return to the main program and it
calls the intro code for doing some title screen to the user. Once the system
is up I can throw away the intro and startup code and reuse the space.

File 3 is the real application. I decided to go for a drawing program that
I named (sketch). It draws a trail when you move the dot using the joypad.
You can erase by pressing joypad B and change color by pressing joypad A.
This application uses single-buffering only. So it never sets the drawPending
flag for flipping the display buffers.

What if you want to modify Sketch to do something else...

The first thing is probably to start using double buffering. This works
by doing something like:

 while (game is running) {
  advance time in your game
  if (!drawPending) {
   the screen is free. draw your stuff there.
   drawPending = 1;
  }
 }

The interrupt routines running in the resident portion will automatically
switch between the draw page and view page when you set drawPending to 1.

This technique eliminates flicker as we always switch the buffers when the
vertical blanking interrupt occurs.

Then you may want sound effects and background music. There is actually
a module called abcmusic running in the resident portion as well. It is
also interrupt-driven and you can pass music to all channels in the ABC
format.

Optimizing memory...

At some point in time your code will have a hard time fitting in memory.
Then you can start tweaking the lynxcart.cfg file. The address range
0..ff is for zero-page stuff
100..1ff is for the stack
200..__RAM_START__ cannot be used when you start the Lynx. But you can put
data there later. I usually use it as a scratch buffer for loading in
data objects from the cart that are used by my code directly. Typical objects
are text for the screen and pictures. __RAM_START__ cannot be smaller than 400.
BA40..FDFF is reserved for the screen buffers
FE00..FFF8 is reserved for the standard uploader
FFF8..FFFF are the special memory bank vectors

So you can try to make the RAM bank smaller until the linker complains.
Then you look for the linker message. It is kind enough to tell you exact
how many bytes are missing from the RAM segment.

Once the RAM is in place yyou can move the starting address down for the
SKETCH bank etc...

In Stardreamer I have more than 150 files already. There are also common
segments defining the universe that stay in memory while smaller modules
are flipped in and out when you do trading, docking, fighting or flying.

So I wish you nice moments with this cart template.

--
Regards,

Karri Kaksonen
karri@sipo.fi




