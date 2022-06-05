.FEATURE labels_without_colons

.INCLUDE "sun.inc"
.IMPORT sunscrdat

SUNSPEEDRESET = 3

.EXPORT fadesun,_is_sun_fade

fadesun:
 .IMPORTZP ptr1
 .IMPORT _spdcadr;=$2800

 					lda #$10
					sta $d016
					
					lda sunypos
					and #7
					ora #$70
					sta $d011
					ldx sunypos
 					beq nofade
 					
 					cmp #$77
					beq hardscroll
					
					dec sunspeed
 					bpl nofade
 					ldx #SUNSPEEDRESET
 					stx sunspeed

					dec sunypos
					rts
hardscroll: dec sunypos
					lda _spdcadr
					sta jsrdest
					lda _spdcadr+1
					sta jsrdest+1
jsrdest = *+1
          jsr !$0
         
          lda sunsrcptr
          sta ptr1
          clc
          adc #$28
          sta sunsrcptr
          
          lda sunsrcptr+1
          sta ptr1+1
          adc #0
          sta sunsrcptr+1
          
          ldy #(sunwinw-1)
copnewsun:
					lda (ptr1),y
					sta sunscrn+((sunwinh-1)*$28),y
					dey
					bpl copnewsun
nohardscroll:
nofade:   rts
sunypos:
	.BYTE sunwinh*8-8
sunsrcptr:
	.WORD sunscrdat+6
sunspeed:
	.BYTE SUNSPEEDRESET

_is_sun_fade:
	lda sunypos
	rts
;needed for the c main-program