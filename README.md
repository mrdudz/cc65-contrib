# cc65-contrib - Readme

CC65 User Contributions (formerly located at https://sourceforge.net/projects/cc65/files/contrib/)

If you want to contribute your work to this archive, have a look at [Contributing.md](Contributing.md)

--------------------------------------------------------------------------------

## WIP note

This Repository was created from the old archive hosted at sourceforge. This
repo is an ongoing effort to clean that stuff up a bit.

- [DONE] unpack all archives, each into a seperate directory
- [DONE] create Makefiles and provide the 3 standard targets for each
- [DONE] create toplevel makefile

- [TODO] all included sources should be cleaned from whatever is copied from the
standard toolchain/libraries:
        - defines or complete header files. include the respective header files
from the toolchain instead
- [TODO] fix the remaining binaries (see below)
- [TODO] make a github action that checks out cc65 on PR and compiles everything
- [TODO] move this repo to the official cc65 account

--------------------------------------------------------------------------------

## FIXME!

some programs still need some extra love:

```
3dmaze-0.2          support TARGET=c128-vdc, atari800, apple2 in Makefile
circlefracv1-1      support TARGET=c128-vdc, atari800, apple2 in Makefile
darth-vader         support TARGET=c128-vdc, atari800, apple2 in Makefile
drachenv1.1         support TARGET=c128-vdc, atari800, apple2 in Makefile
sierpinski-1.1      support TARGET=c128-vdc, atari800, apple2 in Makefile
tgi-preview-0.0.1   support TARGET=c128-vdc, atari800, apple2 in Makefile
hwclock-1.0         support TARGET=c128, c610 in Makefile

calomin             the binaries (atari800) do not work correctly - there is some
                    problem when the music is enabled.
picoos-demo-0.8.0   the binaries (c64) do not work

aclock-geos         untested

lynx-cart-demo-1.1  does not build, some things need to be updated
```

## generic

* [entp-0.7](entp-0.7)                - library for depacking Amica Paint and Zoomatic pictures (by _Monte Carlos_)
* [fixedpoint](fixedpoint)            - Fixed Point Library (by _Steffen Goerzig_)
* [huffman-1.0](huffman-1.0)          - Huffman encoder and decoder (by _Hannenz_)
* [hwclock-1.0](hwclock-1.0)          - Set and get time of day (by _Stefan Haubenthal_)
* [mmind-1.0.0](mmind-1.0.0)          - A simple C mastermind game (by _Peter Sieg_)
* [speedcode-1.0](speedcode-1.0)      - Speedcode generator-generator (by _Monte Carlos_)
* [sqrtv1.0](sqrtv1.0)                - Procedures for calculating squareroots of 8 an 16 bit numbers (includes c64 specific ECM color cycling example)
* [sudoku65](sudoku65)                - Sudoku solver (by _Juergen Buchmueller_, _Stefan Haubenthal_)

## C64 programs

* [autorun-1.5.0](autorun-1.5.0)           - Example on making an autostart program for the C64 (by _Michael McIntosh_)
* [loadkoala-1.0](loadkoala-1.0)           - Koala viewer for the C64 (by _MagerValp_)
* [picoos-demo-0.8.0](picoos-demo-0.8.0)   - [picoOS](https://sourceforge.net/projects/picoos/) RTOS - Demonstration Program V0.8.0 for C64
* [se.g.i.t.a.i.-1.0](se.g.i.t.a.i.-1.0)   - Source to the BCC-Party 2007 Release [Se gitai o Ilios](http://noname.c64.org/csdb/release/?id=56368) (by _Monte Carlos_)

## VIC20 programs

* [quikman2k8-2008.11.1](quikman2k8-2008.11.1)   - A hires Pac-Man clone for the unexpanded VIC20 (by _Robert Hurst_)

## PET programs

* [pet_tetris](pet_tetris)                       - Tetris port for the PETs (by _Tim Howe_)

## Atari800 programs

* [calomin](calomin)                             - Caverns of the lost miner is a platform logic game for the Atari800XL (by _BAKTRA_)

## Lynx programs

* [lynx-cart-demo-1.1](lynx-cart-demo-1.1)       - Cart Demo 1.1 for the Atari Lynx (by _Karri Kaksonen_)

## NES programs

* [nescontrollertest-1.0](nescontrollertest-1.0) - NES Controller Input Test (by _Paul Hackmann_)

## uses CONIO

* [circle-1.0](circle-1.0)               - (by _Monte Carlos_)
* [even_odd-1.0](even_odd-1.0)           - Draws a colorful caro effect on the screen (by _Monte Carlos_)
* [isolinev0.6](isolinev0.6)             - Display rotating stripes on the screen (by _Monte Carlos_)
* [matrixcevi](matrixcevi)               - "Matrix" screen saver for the C64 (by _BastetFurry_, _Stefan Haubenthal_)
* [randfade-1.0](randfade-1.0)           - Small demo of how to fade out a textscreen
* [simplemenu](simplemenu)               - A small library that implements a menu system (by _MagerValp_)

## uses TGI

* [3dmaze-0.2](3dmaze-0.2)               - 3D Maze game (by _Stefan Haubenthal_)
* [circlefracv1-1](circlefracv1-1)       - Recursively drawing of circles (by _Monte Carlos_, _Oliver Schmidt_)
* [darth-vader](darth-vader)             - draws the Darth Vader mask (by _Stefan Haubenthal_)
* [drachenv1.1](drachenv1.1)             - non-recursive algorithm to draw a dragon curve (by _Monte Carlos_)
* [sierpinski-1.1](sierpinski-1.1)       - Sierpinski Triangle
* [tgi-preview-0.0.1](tgi-preview-0.0.1) - preview of the (back then) "upcoming" TGI features (slightly patched for current TGI) (by _Uz_)

## uses GEOS

* [aclock-geos](aclock-geos)             - Port of [aclock](https://github.com/tenox7/aclock) to 6502 GEOS on the Apple II and Commodore 64/128 (by _Peter Godwin_)

---

Some programs/directories formerly contained in this archive were removed, for example because their content was (or is on it's way to be) merged into the main cc65 tree:

* [c64-rrr-1.0](https://github.com/mrdudz/cc65/tree/rremd) - extended memory driver for the retro replay (waiting for rework/merge) (by _Hannenz_)
