.FEATURE labels_without_colons

.EXPORT _kickborder

SWITCH23LINE = 250
SWITCH23LINE2 = 249
SWITCH23LINE3 = 248
SWITCH25LINE = 252
LASTPALLINE = 311
BORDERSPEED = 2

_kickborder:
	sei
	
	lda $d020
	sta bordercolor
	lda $d021
	sta bgcolor
	
:	inc $d012
	bne :-
:	lda $d011
	bmi :- ;wait to start on the top of the screen
	
kickborderloop:
	lda uppersplity1
: cmp $d012
	bne :-
	
	ldx #9
:	dex
	bne :-
	
	lda bordercolor
	sta $d021
	sta $d020
	
	lda uppersplity2
: cmp $d012
	bne :-
 
	lda bgcolor
	sta $d021
	
	lda #SWITCH23LINE
:	cmp $d012
	bne :-
	
	jsr setd011
	pha
	
	lda lowersplity
: cmp $d012
	bne :-
	
	lda bordercolor
	sta $d021
	sta $d020
	
waitlastpalline:
	lda #<LASTPALLINE
:	cmp $d012
	bne :-
		
	lda bgcolor
	sta $d021
	sta $d020
	
	pla
	ora #$18
	sta $d011
	
	.REPEAT BORDERSPEED
	  jsr incsplitlines
	.ENDREP
	lda uppersplity2
	cmp #SWITCH23LINE
	bcs kloop2
	
	jmp kickborderloop
;- - - - - - - - - - - - - - - - - - - - - - - - -	
kloop2:
	lda uppersplity1
:	cmp $d012
	bne :-
		
	ldx #9
:	dex
	bne :-
	
	lda bordercolor
	sta $d021
	sta $d020
	
	lda #SWITCH23LINE2
:	cmp $d012
	bne :-
	
	jsr setd011
	pha
	
	lda uppersplity2
: cmp $d012
	bne :-
 
	lda bgcolor
	sta $d021
		
	lda #<LASTPALLINE
:	cmp $d012
	bne :-
	
	pla
	ora #$18
	sta $d011
	
	lda bgcolor
	sta $d020
	
	.REPEAT BORDERSPEED
		jsr incsplitlines
	.ENDREP
	
	lda uppersplity1
	cmp #SWITCH23LINE2
	bcs kloop3
	
	jmp kloop2
;- - - - - - - - - - - - - - - - - - - - - - - - -	
kloop3:
	lda #SWITCH23LINE2
:	cmp $d012
	bne :-
	
	jsr setd011
	pha
	
	lda uppersplity1
:	cmp $d012
	bne :-
		
	ldx #9
:	dex
	bne :-
	
	lda bordercolor
	sta $d021
	sta $d020
	
	lda uppersplity2
: cmp $d012
	bne :-
 
	lda bgcolor
	sta $d021
		
	lda #<LASTPALLINE
:	cmp $d012
	bne :-
	
	pla
	ora #$18
	sta $d011
	
	lda bgcolor
	sta $d020
	
	.REPEAT BORDERSPEED
	  jsr incsplitlines
	.ENDREP
	
	lda uppersplity2
	cmp #<(LASTPALLINE-(1*BORDERSPEED)-1)
	bcc :+
	lda uppersplity2+1
	cmp #>(LASTPALLINE-(1*BORDERSPEED)-1)
	bcc :+
	.REPEAT BORDERSPEED
		dec uppersplity2
	.ENDREP
:	lda uppersplity1
	cmp #<(LASTPALLINE-(2*BORDERSPEED)-1)
	bcc kloop3
	lda uppersplity1+1
	cmp #>(LASTPALLINE-(2*BORDERSPEED)-1)
	bcs end
	jmp kloop3
;---------------------------------------------------	
incsplitlines:
	lda lowersplity
	cmp #<(LASTPALLINE-1)
	beq :+
	inc lowersplity
:	inc uppersplity1
	bne :+
	inc uppersplity1+1
:	inc uppersplity2
	bne end
	inc uppersplity2+1
end:	rts
;---------------------------------------------------
setd011:
	lda uppersplity1
	and #7
	ora #$10
	sta $d011
	rts
;---------------------------------------------------
.DATA
lowersplity:
	.BYTE 253
uppersplity1:
	.WORD 0
uppersplity2:
	.WORD $33
.BSS
bgcolor:
	.RES 1
bordercolor:
	.RES 1
