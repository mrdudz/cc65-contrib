.EXPORT _copy2ndpart
.IMPORT __PART2_LOAD__, __PART2_SIZE__
.IMPORT __PART2DEST_LOAD__

.IMPORTZP ptr1, ptr2

.SEGMENT "COPYCODE"

_copy2ndpart:
	sei
	
	lda #$34
	sta 1
	
	lda #<rti_code
	sta $fffa
	lda #>rti_code
	sta $fffb
	;also while decruching, there should not be
	;disturbment by pressing restore
	
	lda ptr1
	pha
	lda ptr1+1
	pha
	lda ptr2
	pha
	lda ptr2+1
	pha

	lda #<(__PART2_LOAD__+2)
	sta ptr1
	lda #>(__PART2_LOAD__+2)
	sta ptr1+1

	lda #<(__PART2DEST_LOAD__)
	sta ptr2
	lda #>(__PART2DEST_LOAD__)
	sta ptr2+1
		
	ldx #<((__PART2_SIZE__-2+255)/256);if needed copy one more page
	;substract 2 bytes, because we don't want to copy the start adress
	ldy #0
: lda (ptr1),y
	sta (ptr2),y
	iny
	bne :-
	
	inc ptr1+1
	inc ptr2+1
	
	dex
	bne :-
	
	lda #$37
	sta 1
	
	pla 
	sta ptr2+1
	pla
	sta ptr2
	pla
	sta ptr1+1
	pla
	sta ptr1
	
	pla
	pla

	jmp $080d
rti_code:
	rti
.CODE


	
