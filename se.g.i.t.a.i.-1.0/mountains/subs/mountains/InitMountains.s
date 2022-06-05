.FEATURE labels_without_colons

.INCLUDE "mountains.inc"

.EXPORT _InitMountains
.PROC _InitMountains
	.IMPORT _mountaincol1
	;This Procedure copies the -Chars and the Mountain-Colors 
	;from bergdata and bergcolordata to the visible Screen Area at 
	;bergscrladr and bergcoladr respectively.
	;(Berg is the german word for mountain. I sometimes mix up german
	; and english words in labels, accidently ;) )
          ldx #(<(mountainheight)*40)
setbergd8col
					ldy bergcolordata-1,x
				  lda _mountaincol1,y
          sta bergcoladr-1,x
          
          lda bergdata-1,x
          sta bergscrladr-1,x
          dex
          bne setbergd8col

          rts
.ENDPROC

bergdata:
.INCBIN "datafiles/bergscrdata"
bergcolordata:
.INCBIN "datafiles/bergcolordata"
.EXPORT bergdata, bergcolordata
