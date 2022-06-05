.export initsqr,_initsqr,nextsqr,_nextsqr

;the sqr(x) is the sum off all odd numbers less than N. N is 2*x-1.
;init sqr sets these values. nextsqr calculates the next one by adding
;the odd number which is now to add.
;this works of course only for natural numbers and their negatives.

_initsqr:
.PROC initsqr
	bcc :+
	lda #1
	ldx #0
	ldy #0
:	stx sqr
	sty sqr+1
	sta add
	rts
.ENDPROC

_nextsqr:
.PROC nextsqr
	lda sqr
	clc
	adc add
	sta sqr
	bcc :+
	inc sqr+1
:	ldy sqr+1
	tax
	inc add
	inc add
	lda add
	rts
.ENDPROC

add: .RES 1
sqr: .RES 2
