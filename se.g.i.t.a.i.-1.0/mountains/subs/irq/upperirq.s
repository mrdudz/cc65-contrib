.FEATURE labels_without_colons

.INCLUDE "sun.inc"
.INCLUDE "irq.inc"

.EXPORT upperirq
.PROC upperirq	
	.IMPORT setclowds
					lsr $d019
					pha
					txa
					pha
					tya
					pha
					
					lda #sund018val
          sta $d018
          
          lda #suncol
          sta $d021
          lda #sunbgcol2
          sta $d022
          lda #sunbgcol3
          sta $d023
					
					ldx #1
          dex
          bne *-1
          bit 2 ;this is an important timing loop!
          
					lda $d011
          and #$1f
          cmp #$17
          beq :+
          cmp #$10
          beq :+
          
          ldx #10
          dex
          bne *-1
:				
					sta $d011;dependent on the d011 lower bits
									;choose to wait longer or not
									;in effect we have clean upper edge
					lda #<glattirq
          sta $fffe
          lda #>glattirq
          sta $ffff

          lda #bergirqlin-1
          sta $d012
             
          .IF ::DEBUG<>0
          	inc $d020
          	lda #upperirqline+10
          	cmp $d012
          	bne *-3
          	dec $d020
          .ENDIF
                    
          pla
					tay
					pla
					tax
					pla
					rti
.ENDPROC 

.PROC glattirq
	.IMPORT bergirq
					inc $d012
	
					lsr $d019
					pha
					lda #<bergirq
          sta $fffe
          lda #>bergirq
          sta $ffff
          cli
          nop
          nop
          nop
          nop
          nop
          nop
          nop
          nop
          nop
          nop
          nop
          nop
          nop
 					nop
			
.ENDPROC

