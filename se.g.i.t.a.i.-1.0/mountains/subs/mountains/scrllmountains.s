.FEATURE labels_without_colons

.INCLUDE "mountains.inc"

.IMPORT __BERGSPR_LOAD__
.IMPORT vicbank

.EXPORT scrllmountains
.EXPORT defs, msb, mountainxcor
.EXPORT pottb
 	
scrllmountains
	
					ldx #7
setsprxkoord:
 					ldy mountainxcor,x
 					bne :++
 					
 					lda msb
 					and pottb,x
 					bne :+
 					
 					ldy #<(348)
:
 					lda msb
 					eor pottb,x
 					sta msb
:					dey
				  tya
 					sta mountainxcor,x
 					dex
 					bpl setsprxkoord
 					
		      rts
defs
 .byte <bergsprdef, <bergsprdef+1, <bergsprdef+2, <bergsprdef+3
 .byte <bergsprdef+4, <bergsprdef+5, <bergsprdef+6, <bergsprdef+7
msb
 .byte msbsprbit;$c0+msbsprbit
mountainxcor
 .byte <(bergsprx+0*bergsprxsize)
 .byte <(bergsprx+1*bergsprxsize)
 .byte <(bergsprx+2*bergsprxsize)
 .byte <(bergsprx+3*bergsprxsize)
 .byte <(bergsprx+6*bergsprxsize)
 .byte <(bergsprx+7*bergsprxsize)
 .byte <(bergsprx+8*bergsprxsize)
 .byte <(bergsprx+9*bergsprxsize)

pottb: .byte 1,2,4,8,16,32,64,128


