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

	.include "lynx.inc"
	.include "../resident/extzp.inc"
		.export     _read_cart_dir
		.export     FileLoadDir
		.import     FileReadByte, FileReadOver, FileSelectBlock
	.data
diroff:		.res	2

	.code

;*******************************************
; void LoadCartDir(int bFileNr)
; void read_cart_dir(int file_num);
; loads the specified directory entry
; into the global structure FileEntry
_read_cart_dir:
		jsr FileLoadDir ; fill the global structure FileEntry 
		rts

;**********************************
; Load directory-entry
; In: Accu = entry number
;**********************************
FileLoadDir:    stz	diroff	; Multiply entry number by 8
		stx	diroff+1
		clc
		rol
		rol	diroff+1
		clc
		rol
		rol	diroff+1
		clc
		rol
		rol	diroff+1

		clc             ; over-read the cart-header
		adc	#<410
		sta	diroff
		lda	diroff+1
		adc	#>410
		sta	diroff+1

		clc
		ror		; get whole blocks
		clc
		ror
		sta	_FileCurrBlock
		jsr	FileSelectBlock

		lda	diroff
		eor	#$ff
		tax
		lda 	diroff+1
		and	#3
		eor	#$ff
		tay
		jsr FileReadOver

		ldx #0
		ldy #8
loopLD:         jsr FileReadByte
		sta _FileEntry,x
		inx
		dey
		bne loopLD
		rts

