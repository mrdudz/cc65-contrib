.INCLUDE "mountains.inc"

.EXPORT _sprinit

.PROC _sprinit
	.IMPORT clowdsprdest
	
	.IF ::DEBUG = 1
		       ldx #0
				   ldy #0  
		copclspr2:
          lda testspr,y
					sta clowdsprdest,x
					sta clowdsprdest+$c0,x
					iny
					tya
					and #63
					tay
					inx
					bne copclspr2
	.ENDIF
          
					lda #bergspr_mcolbit
					sta $d01c

					lda #bergsprxpand
          sta $d01d
          .if (::bergsprxpand<>::bergsprypand)
          lda #bergsprypand
          .endif
          sta $d017
          rts
.IFDEF ::DEBUG
testspr: .BYTE $cc,$88,$44
				 .BYTE $c0,0,0,$c0,0,0,$c0,0,0
				 .BYTE $c0,0,0,$c0,0,0,$c0,0,0
				 .BYTE $c0,0,0,$c0,0,0
				 .BYTE $cc,$88,$44,0,0,0,$cc,$88,$44
				 .BYTE 0,0,0,0,0,0,0,0,0
				 .BYTE 0,0,0,0,0,0,0,0,0
				 .BYTE 0,0,0,0,0,0,$cc,$88,$44
.ENDIF 
.ENDPROC

;.ENDPROC