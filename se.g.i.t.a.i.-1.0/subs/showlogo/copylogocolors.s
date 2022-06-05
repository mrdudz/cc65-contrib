.EXPORT _copycolram1, _copycolram2
.IMPORT _logocolors

LOGOH = 9
LOGOW = 40

LOGOY1 = 0
LOGOY2 = 16

_copycolram1:

	ldx #0
	lda #0
:	
	sta $d800+((LOGOY1)*LOGOW),x
	inx
	bne :-
:	sta $d900+((LOGOY1)*LOGOW),x
	inx
	cpx #$68
	bne :-
	
	ldx #LOGOW-1
:
.REPEAT LOGOH, repcnt
	lda _logocolors+(repcnt*LOGOW),x
	sta $d800+((LOGOY2+repcnt)*LOGOW),x
.ENDREP
	dex
	bpl :-

	rts

_copycolram2:
	ldx #LOGOW-1
:	
.REPEAT LOGOH, repcnt
	lda _logocolors+(repcnt*LOGOW),x
	sta $d800+((LOGOY1+repcnt)*LOGOW),x
.ENDREP
	dex
	bpl :-

 	rts
	