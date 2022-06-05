.EXPORT _copyclowds
.PROC _copyclowds
;this routine must not change the y-reg, because
;it is called also from the speedcodegeneration
;to avoid break of clowdmovement while the 
;speedcode for the sunsinmovement is generated
	.IMPORT clowdsprdest, _clowdsprend, _clowdspr
					ldx #0
copclspr:
clowdsprsrcptr=*+1
					lda _clowdspr,x
					sta clowdsprdest,x
clowdsprsrcptr2=*+1
					lda _clowdspr+$c0,x
					sta clowdsprdest+$c0,x
					inx
					bne copclspr
					
					lda clowdsprsrcptr+1
					cmp #>(_clowdsprend-(7*64))
					bcs clowdreset
					
					lda clowdsprsrcptr
noclowdreset:
					clc
					adc #<(7*64)
					sta clowdsprsrcptr
					
					lda clowdsprsrcptr+1
					adc #>(7*64)
					sta clowdsprsrcptr+1
					
					lda clowdsprsrcptr2
					clc
					adc #<(7*64)
					sta clowdsprsrcptr2
					
					lda clowdsprsrcptr2+1
					adc #>(7*64)
					sta clowdsprsrcptr2+1
				
					rts
clowdreset:
				  lda #<_clowdspr
				  sta clowdsprsrcptr
				  
				  lda #>_clowdspr
				  sta clowdsprsrcptr+1
				  
				   lda #<(_clowdspr+$c0)
				  sta clowdsprsrcptr2
				  
				  lda #>(_clowdspr+$c0)
				  sta clowdsprsrcptr2+1
				  rts
.ENDPROC