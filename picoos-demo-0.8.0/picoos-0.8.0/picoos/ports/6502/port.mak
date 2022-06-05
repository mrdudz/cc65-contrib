#  Copyright (c) 2004, Dennis Kuschel / Swen Moczarski
#  All rights reserved. 
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#   1. Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   3. The name of the author may not be used to endorse or promote
#      products derived from this software without specific prior written
#      permission. 
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
#  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
#  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
#  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
#  OF THE POSSIBILITY OF SUCH DAMAGE.


#  This file is originally from the pico]OS realtime operating system
#  (http://picoos.sourceforge.net).
#
#  $Id: port.mak,v 1.2 2004/03/13 19:33:26 dkuschel Exp $


# Compiler: Define place of compiler
CC65 = $(shell cl65 --print-target-path)/..

# Compiler: Define target type
TG = c64


# Set to 1 to include generic pico]OS "findbit" function
GENERIC_FINDBIT = 0

# Define extensions
EXT_C   = .c
EXT_ASM = .s
EXT_OBJ = .o
EXT_LIB = .lib
EXT_OUT = .$(TG)

# Define tools: compiler, assembler, archiver, linker
CC = $(CC65)/bin/cc65
AS = $(CC65)/bin/ca65
AR = $(CC65)/bin/ar65
LD = $(CC65)/bin/ld65

# Define to 1 if CC outputs an assembly file
CC2ASM = 1

# Define general options
OPT_CC_INC   = -I
OPT_CC_DEF   = -D
OPT_AS_INC   = -I
OPT_AS_DEF   = -D
OPT_AR_ADD   =
OPT_LD_SEP   = 
OPT_LD_PFOBJ = 
OPT_LD_PFLIB = 
#OPT_LD_FIRST = $(CC65)/lib/$(TG).o
OPT_LD_FIRST =
OPT_LD_LAST  = $(CC65)/lib/$(TG).lib

# Set global defines for compiler / assembler
CDEFINES =
ADEFINES =

# Set global includes
CINCLUDES = $(CC65)/include .
AINCLUDES = $(CC65)/include .

# Distinguish between build modes
ifeq '$(BUILD)' 'DEBUG'
  CFLAGS   += -g
  AFLAGS   += -g
  CDEFINES += _DBG
  ADEFINES += _DBG
else
  CDEFINES += _REL
  ADEFINES += _REL
endif

# Define Compiler Flags
CFLAGS += -t $(TG) -O -T -o 

# Define Assembler Flags
ASFLAGS += -t $(TG) -o 

# Define Linker Flags
LDFLAGS = -t $(TG) -Ln $(DIR_OUT)/$(TARGET).lbl -m $(DIR_OUT)/$(TARGET).map -o 
#LDFLAGS = -t $(TG) -o 

# Define archiver flags
ARFLAGS = a 
