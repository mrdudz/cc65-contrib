.FEATURE labels_without_colons

.INCLUDE "sun.inc"
.INCLUDE "clowds.inc"
.INCLUDE "irq.inc"

.EXPORT bergirq
.PROC bergirq		
	.IMPORT showrbar, setmountainspr, scrllmountains
	.IMPORT boffirq, xpos, vicadr, _mountaincol2
	.IMPORT _clowdbgcol
	
					lsr $d019

					lda #$1f
          sta $d011
					lda #sund018val2
          sta $d018	
          
          lda _clowdbgcol
					sta $d021
					lda _mountaincol2
					sta $d023
					
					pla
					pla
					pla
							
					;don't use x and y up to here!
					;push a is in glattirq!!!
					
					txa
					pha
					tya
					pha
  .IF ::DEBUG
					inc $d020
	.ELSE
					lda (0,x)
	.ENDIF
          lda #<boffirq
          sta $fffe
          lda #>boffirq
          sta $ffff

          lda #boffirqlin
          sta $d012
					
					lda xpos
          ora #$10
          sta $d016
          					
          jsr setmountainspr
          
          jsr showrbar
           jsr scrllmountains
.IF ::DEBUG
					dec $d020
					dec $d020
.ELSE
					lda(0,x)
					lda(0,x)
	.ENDIF
          pla
          tay
          pla
          tax
          pla
          rti
.ENDPROC         