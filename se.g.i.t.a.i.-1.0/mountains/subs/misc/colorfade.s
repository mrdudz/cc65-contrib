.INCLUDE "clowds.inc"
.INCLUDE "mountains.inc"

FADESPEEDRESET = 40

.EXPORT _clowdsprcolor, _clowdsprcolor2, _clowdsprcolor3, _clowdbgcol
.EXPORT _bergsprcol, _bergsprcol2, _bergsprcol3
.EXPORT _mountaincol1,_mountaincol2, _mountaincol3

.EXPORT fadecolors
fadecolors:
	dec fadespeed
	bpl nofade
	
	lda #FADESPEEDRESET
	sta fadespeed
	
	ldx fadepos
	cpx #fadelen
	beq nofade
	
	lda _clowdsprcolor,x
	sta _clowdsprcolor
	
	lda _clowdsprcolor2,x
	sta _clowdsprcolor2
	
	lda _clowdsprcolor3,x
	sta _clowdsprcolor3
	
	lda _clowdbgcol,x
	sta _clowdbgcol
	
	lda _bergsprcol,x
	sta _bergsprcol
	
	lda _bergsprcol2,x
	sta _bergsprcol2
	
	lda _bergsprcol3,x
	sta _bergsprcol3
	
	lda _mountaincol1src,x
	sta _mountaincol1
	
	lda _mountaincol2,x
	sta _mountaincol2
	
	lda _mountaincol3src,x
	sta _mountaincol3
	
	inc fadepos
nofade:
	rts
fadepos:
	.BYTE 1
fadespeed:
	.byte FADESPEEDRESET
	
_clowdsprcolor:
	.BYTE 12,10,3,7,13,1
fadelen = *-_clowdsprcolor
_clowdsprcolor2:
	.BYTE 11,11,11,11,11,12;6,6,6,4,4,12
_clowdsprcolor3:
	.BYTE 11,12,12,10,15,15
_clowdbgcol:
	.BYTE 14,14,14,14,14,14
	
_bergsprcol:
	.BYTE 11,11,11,12,10,15
_bergsprcol2:
	.BYTE 9,9,9,9,11,11
_bergsprcol3:
	.BYTE 0,0,0,9,9,9

_mountaincol1:
 .BYTE 8
_mountaincol3:
 .BYTE 8
_mountaincol1src: 
	.BYTE 8,8,8,4+8,3+8,7+8;7+8
_mountaincol3src:
	.BYTE 8,8,6+8,6+8,6+8,2+8
;must stay in this order, because _mountaincol1 ist read indexed
_mountaincol2:
	.BYTE 0,6,11,11,12,10


