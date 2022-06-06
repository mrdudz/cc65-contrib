
calomin-1.0-D 
Caverns of the lost miner is a platform logic game
Written by Michael Kalous in 2007. Contact: zylon@post.cz
Developed using cc65 2.11.0

This game runs only on eight bit Atari (800XL and better).

The game demonstrates, that with cc65 you can easily develop
a game which has:

- sounds and music
- graphics
- joystick control
- assembler subroutines
- sprites

Caverns of the lost miner - how to customize
============================================

1. Software you need
====================
- GNU/Linux is strongly recommended, but you can use also Cygwin under Windows
  or convert shell scripts into batch files
- CC65 C cross compiler
- Java Runtime Environment 1.3 for running utilites minilinker and lowerbit

2. How to edit
==============
- You can edit caves by using calomin_editor, download at game web page
- You can edit graphics by editing font located at data/clmfont.fnt



Caverns of the lost miner
A logical game for 8-bit atari by BAKTRA 
========================================

1.Game goal
===========
Your task is to guide the lost miner through 13 dangerous caves

2.Controls
==========
Use keyboard at title screen:
START - start game
C     - select starting cave (the cave must be reached in previous game)
D     - select difficulty
ESC   - COLD START

Use keyboard and joystick in the caves:
START  - return to menu
OPTION - instant death

JS - move the lost miner
FIRE + JS - three sorts of jump

3.Running from TAPE or DISK
===========================
- Since version C, the memory layout of the program has been rearanged, so this
  program works with (at least) DOS 2.5. DOS XE, TOS 4.1 BL+, FLOP DOS. Running
  from disk is now easy.

- calomin_tape.xex is dedicated to tape recorder users. Too slow with unmodified
  tape recorder. Works also with Turbo 2000 or Super Turbo.

4.Other info
============
This game is little bit lame, take into account I am not experienced 8-bit
Atari programmer. The game uses the following "techniques" : ANTIC display
list programming,Player Missille graphics, VBI,DLI. Audio is controlled by
Raster Music Tracker (RMT) playback routine.

The game is based on game "Lost miner" (for IBM/PC) and "Lost miner" is
based on "Super Miner" (for ATARI by Byte nibbler). So this is not a port
or a backport, but circleport :-)

5.Other Atari related software by baktra
========================================
* mmSAP - comfortable SAP player for GNU/Linux
* TURGEN SYSTEM 4 - Data transfer utility PC/SUN --> ATARI

6.Contact information
=====================
email  : zylon@post.cz 
Website: www.baktra.wz.cz/index.html   (for atarians from former Czechoslovakia)
         www.baktra.wz.cz/english.html (for other atarians)

7.Copying
=========
This game is released under GNU LGPL. The tools minilinker and lowerbit are public domain
This game comes to you from Czech Republic.
