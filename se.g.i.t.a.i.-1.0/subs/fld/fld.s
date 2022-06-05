.EXPORT _fld
.AUTOIMPORT on

FLD_START_Y= $30

.SEGMENT "FLDCODE"
_fld: sta modemod
	
	jsr popa
	
	ldx #FLD_START_Y
:	cpx $d012
	bne :-
	tax
	beq endfld
	
fldloop:	
	lda $d012
	clc
	adc #3
	and #7
modemod=*+1
	ora #$00
	sta $d011;16zyk
	
	ldy #6;18
:	dey
	bne :-;47
	bit 2
	nop;51

	lda (0,x)
	
	dex
	bne fldloop

endfld:
	rts
	