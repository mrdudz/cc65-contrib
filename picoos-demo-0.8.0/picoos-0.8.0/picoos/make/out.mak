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
#  $Id: out.mak,v 1.1.1.1 2004/02/16 20:11:22 smocz Exp $


# Build target: generate executable

ifeq '$(strip $(MAKE_OUT))' ''
($error common.mak not included)
endif

ifeq '$(strip $(TARGET))' ''
($error TARGET output name not set)
endif

# ---------------------------------------------------------------------------

# Set source files
SRC := $(SRC_TXT)

# Clear object file list
OBJ :=

# Add pico]OS library
SRC_LIB += $(DIR_LIB)/picoos$(EXT_LIB)

# Compile source files
include $(MAKE_CPL)

# ---------------------------------------------------------------------------

# Build pico]OS library

.PHONY: $(DIR_LIB)/picoos$(EXT_LIB)
$(DIR_LIB)/picoos$(EXT_LIB):
	$(MAKE) -C $(RELROOT) --no-print-directory all

# ---------------------------------------------------------------------------

# Call linker to build the executable

empty=
space=$(empty) $(empty)
ifeq '$(strip $(OPT_LD_SEP))' ''
replace=$(space)
else
replace=$(OPT_LD_SEP)
endif

TARGETOUT=$(DIR_OUT)/$(TARGET)$(EXT_OUT)
OBJLIST  =$(strip $(OBJ)) $(strip $(SRC_OBJ))
LIBLIST  =$(SRC_LIB)
LINKLIST =$(OPT_LD_FIRST)
LINKLIST+=$(OPT_LD_PFOBJ)$(subst $(space),$(replace),$(strip $(OBJLIST)))
LINKLIST+=$(OPT_LD_PFLIB)$(subst $(space),$(replace),$(strip $(LIBLIST)))
LINKLIST+=$(OPT_LD_LAST)

$(TARGETOUT): $(OBJLIST) $(LIBLIST) $(DIR_OUT) $(COMMONDEP)
	$(PRELINK1)
	$(PRELINK2)
	$(PRELINK3)
	$(LD) $(LDFLAGS)$(call adjpath,$@) $(call adjpath,$(LINKLIST))
	$(POSTLINK1)
	$(POSTLINK2)
	$(POSTLINK3)

# ---------------------------------------------------------------------------

ifneq '$(PORT)' ''

# Set the library as default target.
all: $(TARGETOUT)

# Target: clean executable.
clean: $(CLEAN_USER)
	-$(REMOVE) $(TARGETOUT)
	-$(REMOVE) $(OBJ)

endif
