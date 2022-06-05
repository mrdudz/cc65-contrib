.FEATURE labels_without_colons

.INCLUDE "mountains.inc"
.INCLUDE "sun.inc"

rtscode=$60
LDA_XIND = $bd
LDA_ABS = $ad

.IMPORTZP ptr1
.IMPORT _malloc

.SCOPE
.IMPORT sunscrdat
 
.EXPORT spdcgen
.PROC spdcgen
;This sub copies sunwinw*sunwinh times the short 
;code sequence between spdc and spdce.
;To pass registers as parameters a function must be declared as fastcall
;in the header file. Then a/x contains the value of the rightmost parameter
;after the call.
.IMPORT _copyclowds
 					sta ptr1
 					stx ptr1+1
 					bcc onlyscroll
 					;if carry clear, then generate speedcode for scrolling
 					;if carry set, then generate speedcode for sin movement
 					lda #<(sunscrdat)
 					sta spdcsrcmod+1
 					lda #>(sunscrdat)
 					sta spdcsrcmod+2
 					
 					lda #LDA_XIND
 					sta spdcsrcmod
 					
 					lda #<(sunwinw*sunwinh-1)
 					sta cursunpos
 					lda #>(sunwinw*sunwinh-1)
 					sta cursunpos+1
 					
 					bne bothspeedcodes
onlyscroll:
					lda #<(sunscrn+$28)
 					sta spdcsrcmod+1
 					lda #>(sunscrn+$28)
 					sta spdcsrcmod+2
 					
 					lda #LDA_ABS
 					sta spdcsrcmod
 					
 					lda #<(sunwinw*(sunwinh-1)-1)
 					sta cursunpos
 					lda #>(sunwinw*(sunwinh-1)-1)
 					sta cursunpos+1
bothspeedcodes:
					lda #<sunscrn
					sta spdcdestmod+1
					lda #>sunscrn
					sta spdcdestmod+2
sgenlop
          ldy #<(spdce-spdc-1)
copspdc
          lda spdc,y
          sta (ptr1),y
          dey
          bpl copspdc

          lda ptr1
          clc
          adc #<(spdce-spdc)
          sta ptr1
          bcc *+4
          inc ptr1+1

          inc spdcsrcmod+1
          bne *+5
          inc spdcsrcmod+2

          inc spdcdestmod+1
          bne *+5
          inc spdcdestmod+2
					
					lda $dd0d
					and #1
					beq noclowdmove
					
					jsr _copyclowds
noclowdmove:
;bit 0 of $dd0d is 1, if timer for clowd-movement
;underruns.
          dec cursunpos
          lda cursunpos
          cmp #$ff
          bne sgenlop
          dec cursunpos+1
          bpl sgenlop

          ldy #0
          lda #rtscode
          sta (ptr1),y
          rts
cursunpos
 	.word ( (sunwinw*sunwinh)-1 )
.ENDPROC

.EXPORT _createspeedcode1
.PROC _createspeedcode1
	.IMPORTZP tmp1, tmp2
	sec
	sbc #1
	php
	
	lda #<(sunwinw*sunwinh*(spdce-spdc)+1)
	ldx #>(sunwinw*sunwinh*(spdce-spdc)+1)
	jsr _malloc
	sta tmp1
	stx tmp2
	plp
	jsr spdcgen
	;must not change tmp1 and tmp2
	
	lda tmp1
	ldx tmp2
	rts
.ENDPROC 

spdc
spdcsrcmod
          lda sunscrdat,x
spdcdestmod
          sta sunscrn
spdce

.ENDSCOPE

;-------------------------------------------

.SCOPE
	.IMPORT bergcolordata
	.IMPORT _mountaincol1
	
.EXPORT spdcgen2
.PROC spdcgen2
			    sta ptr1
	        ;pha
          stx ptr1+1
          ;txa
          ;pha
sgenlop0:        
          lda #SCRW-1
          sta curmountainx
sgenlop
          ldy #<(spdce-spdc-1)
copspdc
          lda spdc,y
          sta (ptr1),y
          dey
          bpl copspdc

          lda ptr1
          clc
          adc #<(spdce-spdc)
          sta ptr1
          bcc *+4
          inc ptr1+1

          inc spdcsrcmod+1
          bne *+5
          inc spdcsrcmod+2

          inc spdcdestmod+1
          bne *+5
          inc spdcdestmod+2

          dec curmountainx
          bpl sgenlop
          
          lda spdcsrcmod+1
          clc
          adc #mountainxsize-1
          sta spdcsrcmod+1
          bcc *+5
          inc spdcsrcmod+2

          dec curmountainy
          bpl sgenlop0

          ldy #0
          lda #rtscode
          sta (ptr1),y
          
          ;pla
          ;tax
          ;pla
          rts
curmountainx:
	.byte SCRW-1
curmountainy:
	.BYTE mountainheight-1
.ENDPROC

.EXPORT _createspeedcode2
.PROC _createspeedcode2
	.IMPORTZP tmp1, tmp2
	lda #<(mountainxsize*mountainheight*(spdce-spdc)+1)
	ldx #>(mountainxsize*mountainheight*(spdce-spdc)+1)
	jsr _malloc
	sta tmp1
	stx tmp2
	jsr spdcgen2
	;must not change tmp1 and tmp2
	
	lda tmp1
	ldx tmp2
	rts
.ENDPROC 

spdc 		
spdcsrcmod
					ldx bergcolordata,y
					lda _mountaincol1,x
spdcdestmod
					sta bergcoladr
spdce

.ENDSCOPE
	


