.FEATURE labels_without_colons

.INCLUDE "clowds.inc"

clowdspeed = 2

.EXPORT scrollclowds
.PROC scrollclowds
	.IMPORT pottb
	;Import from scrollmountains.s
	.EXPORT clowdxkor, clowdmsb
					dec speedcntr
					bne end
					
					lda #clowdspeed
					sta speedcntr
					
					ldx #7
setsprxkoord:
					lda clowdmsb
					ldy clowdxkor,x
					php
					and pottb,x
					beq nosprmsb
					
					plp
 					beq changemsb
 					cpy #<(8*48)
 					bcc normaldec
 					cpy #<($1e0+1)
					bcs normaldec
					
					jsr scrldef
					ldy #<(7*48+24)
 					bne normaldec

nosprmsb: plp
					bne normaldec

					ldy #<($1f8)
changemsb:
					lda clowdmsb
 					eor pottb,x
 					sta clowdmsb
normaldec:
					dey
				  tya
 					sta clowdxkor,x
 					dex
 					bpl setsprxkoord
end:	
		      rts
clowdxkor:
	.BYTE <(CLOWDSPRX+(0*CLOWDSPRXSIZE)), <(CLOWDSPRX+(1*CLOWDSPRXSIZE))
	.BYTE <(CLOWDSPRX+(2*CLOWDSPRXSIZE)), <(CLOWDSPRX+(3*CLOWDSPRXSIZE))
	.BYTE <(CLOWDSPRX+(4*CLOWDSPRXSIZE)), <(CLOWDSPRX+(5*CLOWDSPRXSIZE))
	.BYTE <(CLOWDSPRX+(6*CLOWDSPRXSIZE)), <(CLOWDSPRX+(7*CLOWDSPRXSIZE))
clowdmsb:
	.BYTE %11100000
speedcntr:
	.BYTE clowdspeed
.ENDPROC
;---------------------------------------
.PROC scrldef
	.IMPORT __CLOWDSPRBUF_LOAD__
	.IMPORT vicbank
	.EXPORT clowddef
          lda cursprdef
          sta clowddef,x
          inc cursprdef
          cmp #<( (__CLOWDSPRBUF_LOAD__-vicbank)/$40+6 )
          bne noresetdef
          lda #<( (__CLOWDSPRBUF_LOAD__-vicbank)/$40 )
          sta cursprdef
noresetdef
          rts
cursprdef
 .byte <( (__CLOWDSPRBUF_LOAD__-vicbank)/$40+1 )
clowddef:
	.BYTE <( (__CLOWDSPRBUF_LOAD__-vicbank)/$40 )
	.BYTE <( (__CLOWDSPRBUF_LOAD__-vicbank)/$40+1 )
	.BYTE <( (__CLOWDSPRBUF_LOAD__-vicbank)/$40+2 )
	.BYTE <( (__CLOWDSPRBUF_LOAD__-vicbank)/$40+3 )
	.BYTE <( (__CLOWDSPRBUF_LOAD__-vicbank)/$40+4 )
	.BYTE <( (__CLOWDSPRBUF_LOAD__-vicbank)/$40+5 ) 
	.BYTE <( (__CLOWDSPRBUF_LOAD__-vicbank)/$40+6 )
	.BYTE <( (__CLOWDSPRBUF_LOAD__-vicbank)/$40+0 )
.ENDPROC


