.EXPORT _irqinit
.INCLUDE "irq.inc"

.PROC _irqinit
	.IMPORT upperirq, boffirq
	lda #$7f
  sta $dc0d
  bit $dc0d

  lda #$81
  sta $d01a

  lda #boffirqlin;upperirqline
  sta $d012
						          
  lda #<boffirq;upperirq
  sta $fffe
  lda #>boffirq;upperirq
  sta $ffff
  
  lda #<rti_code
  sta $fffa
  lda #>rti_code
  sta $fffb
  
  lda #$0b
  sta $d011
  
  lsr $d019
  rts
rti_code:
  rti
.ENDPROC