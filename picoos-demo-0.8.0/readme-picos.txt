
         pico]OS RTOS - Demonstration Program V0.8.0  for C64


 pico]OS is a highly configurable and very fast real time operating system.
 It targets a wide range of architectures, from very small 8 bit processors
 and  microcontrollers  up to very huge platforms. Ports are  available for
 6502, 80x86, PPC and AVR. 

 This zip file contains a program that demonstrates multitasking on the
 Commodore 64.

 snakes.c64     - compiled demonstration program, ready to load into VICE
 snake-src.zip  - archive with C source of the demo
 readme.txt     - this file

 The pico]OS realtime operating system can be downloaded at
 http://sourceforge.net/projects/picoos/
 
 $ wget https://sourceforge.net/projects/picoos/files/picoos/picoos-1.0.4/picoos-1.0.4.zip/download

 $ mkdir -p picoos-1.0.4/contrib/demos/6502-c64/snakes
 $ cp *.c picoos-1.0.4/contrib/demos/6502-c64/snakes/
 $ cp *.h picoos-1.0.4/contrib/demos/6502-c64/snakes/
 $ cp Makefile picoos-1.0.4/contrib/demos/6502-c64/snakes/
 
 You need CC65 version 2.10.0-1 to compile pico]OS and the demo.

 Please send questions and comments to  dennis_k@freenet.de


-----------------------------------------------------------------------------

  Copyright (c) 2004, Dennis Kuschel.
  All rights reserved. 

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   3. The name of the author may not be used to endorse or promote
      products derived from this software without specific prior written
      permission. 

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
  OF THE POSSIBILITY OF SUCH DAMAGE.

-----------------------------------------------------------------------------
