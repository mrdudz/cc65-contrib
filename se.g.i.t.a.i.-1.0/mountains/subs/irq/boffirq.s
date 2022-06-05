.FEATURE labels_without_colons

.INCLUDE "mountains.inc"
.INCLUDE "irq.inc"

.EXPORT boffirq
.PROC boffirq	
	.IMPORT scrollsun, scrollclowds, setclowds
	.IMPORT scrllmountains, txtscrll, fadesun
	.IMPORT _allow_sunsinmove
	
	.IMPORT is_sun_fade, _spdcadr2
	
	.IMPORT upperirq, fadecolors
	.IMPORTZP sund018val
	
					lsr $d019
					
					pha
					txa
					pha
					tya
					pha
					
        	bit 2
          
	.IF ::DEBUG
					inc $d020
	.ELSE
					lda (0,x)
	.ENDIF				
					jsr setclowds
					jsr fadecolors 
							
					lda #<upperirq
					sta $fffe
					lda #>upperirq
					sta $ffff
					lda #upperirqline
					sta $d012
  				
					lda #248
					cmp $d012
					bne *-3

					lda #$13
					sta $d011
          
          jsr scrollclowds		
          
          lda $d011
          bmi :+
          lda #252
          cmp $d012
          bcs *-3
:  
	.IF ::DEBUG           
					inc $d020
	.ELSE
					lda #$73
					sta $d011
	.ENDIF 
					cli
					jsr txtscrll
					lda _allow_sunsinmove
					beq :+
	        jsr scrollsun
	        jsr musicplay
	        jmp nofade
:	
				  jsr fadesun
nofade:		  
	.IF ::DEBUG
					lda #0
					sta $d020
	.ELSE
				  lda (0,x)
	.ENDIF  
					pla
					tay
					pla
					tax
					pla
					rti
.SEGMENT "MUSIC"
_musicinit:
musicplay=*+3
.EXPORT _musicinit
.INCBIN "datafiles/currentmusic.prg"
.ENDPROC
