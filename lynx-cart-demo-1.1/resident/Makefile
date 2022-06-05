#
# makefile for the www.cc65.org compiler version of abccreator
#

CC=cc65 -t lynx -Oirs
CO=co65
RM=rm -f
CP=cp

TARGET?=lynx

TARGETPATH=$(shell cl65 --print-target-path)/$(TARGET)

JOY_DRIVER=$(TARGETPATH)/drv/joy/lynx-stdjoy.joy
TGI_DRIVER=$(TARGETPATH)/drv/tgi/lynx-160-102-16.tgi

.SUFFIXES: .c .s

all:	lynx-stdjoy.s lynx-160-102-16.s resident.s

lynx-stdjoy.s :
	$(CP) "$(JOY_DRIVER)" .
	$(CO) --code-label _lynxjoy lynx-stdjoy.joy

lynx-160-102-16.s :
	$(CP) "$(TGI_DRIVER)" .
	$(CO) --code-label _lynxtgi lynx-160-102-16.tgi

# Component target
resident.s: resident.c
	$(CC) $*.c

clean :
	$(RM) lynx-160-102-16.*
	$(RM) lynx-stdjoy.*
	$(RM) resident.s
	$(RM) *.o
