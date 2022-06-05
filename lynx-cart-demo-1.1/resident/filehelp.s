; ***
; CC65 Lynx Library
;
; Originally by Bastian Schick
; http://www.geocities.com/SiliconValley/Byte/4242/lynx/
;
; Ported to cc65 (http://www.cc65.org) by
; Shawn Jefferson, June 2004
; 
; ***
;
; Helper stuff for the cartridge file functions.

	.include "lynx.inc"
	.include "../resident/extzp.inc"
		.export     FileReadOver, FileReadBytes
		.export     FileReadByte, FileSelectBlock 


	.code


;**********************************
; Overread some bytes
; X:Y count (EOR $FFFF)
;**********************************
FileReadOver:   inx
		bne FROcont0
		iny
		beq exit
FROcont0:       jsr FileReadByte
		bra FileReadOver

;**********************************
; load to DestPtr
; X:Y count (EOR $ffff)
;**********************************
FileReadBytes:  inx
		bne FRBcont1
		iny
		beq exit
FRBcont1:       jsr FileReadByte       ; one byte from the cartridge
		sta (_FileDestPtr)
		inc _FileDestPtr
		bne FileReadBytes
		inc _FileDestPtr+1
		bra FileReadBytes

;**********************************
; Read one byte from cartridge
;**********************************
FileReadByte:   lda $fcb2 ; use CARD0 for reads !!!!!
		inc _FileBlockByte
		bne exit
		inc _FileBlockByte+1
		bne exit


;**********************************
; Select a block 
;**********************************
FileSelectBlock:
		pha
		phx
		phy
		lda __iodat
		and #$fc
		tay
		ora #2
		tax
		lda _FileCurrBlock
		inc _FileCurrBlock
		sec
		bra SLB2

SLB0:           bcc SLB1
		stx $fd8b
		clc

SLB1:           inx
		stx $fd87
		dex

SLB2:           stx $fd87
		rol
		sty $fd8b
		bne SLB0

		lda __iodat
		sta $fd8b
		stz _FileBlockByte
		lda #$100-(>1024)
		sta _FileBlockByte+1
		ply
		plx
		pla

exit:           rts

;###################################################################



