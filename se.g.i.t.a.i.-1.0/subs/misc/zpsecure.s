.EXPORT _savezp, _restorezp

_savezp:
	sta dest
	stx dest+1
	
	ldx #3
: lda 0,x
dest=*+1
	sta $ffff,x
	inx
	bne  :-
	rts

_restorezp:
	sta src
	stx src+1
	
	ldx #3
: 
src=*+1
	lda $ffff,x
	sta 0,x
	inx
	bne  :-
	rts	