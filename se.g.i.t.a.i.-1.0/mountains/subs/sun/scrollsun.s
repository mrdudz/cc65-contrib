.FEATURE labels_without_colons

.include "sun.inc"

.EXPORT scrollsun
.PROC scrollsun
 .IMPORTZP tmp1, tmp2
 .IMPORT _spdcadr;=$2800
 .EXPORT sunscrdat
xdiv8 = tmp1
ymod248 = tmp2
          ldx sunsinpos
          lda sunysin,x
          pha
          and #7
          eor #$77
          sta $d011
					
          ldx suncospos
          lda sunxsin,x
          pha
          and #7
          eor #$17
          sta $d016
          inx
          cpx #sunxsinlen
          bne *+4
          ldx #0
          stx suncospos
			 
          pla
          lsr
          lsr
          lsr
          sta xdiv8
          pla
          and #248
          sta ymod248
          asl
          asl
          clc
          adc xdiv8
          adc ymod248
          tax
          inc sunsinpos
          nop;avoid page border. may be removed
          	 ;if something other changes
          jmp (_spdcadr)
.DATA
sunsinpos
 .byte $b8
suncospos
 .byte 0
sunysin:
.INCBIN "datafiles/sunysin"
sunxsin:
.INCBIN "datafiles/sunxsin"
sunxsinlen= *-sunxsin
sunscrdat:
.INCBIN "datafiles/sunscrdata"
.SEGMENT "SUNCHAR"
sunchr:
.INCBIN "datafiles/suncharset"
.ENDPROC 
