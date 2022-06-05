.import _rt, _lt
.importzp sp
.export _pos,left_or_right

left_or_right:
	lda (sp),y
	pha
	iny
	lda (sp),y
	sta pos+1
	ldx #16
	pla
:	lsr pos+1
	ror 
	bcc endloop
	dex
	bpl :-
endloop:
	lsr pos+1
	ror
	bcc right
	jmp _lt
right:
	jmp _rt

_pos:
pos:
	.res 2
