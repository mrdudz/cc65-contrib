.FEATURE labels_without_colons

.INCLUDE "mountains.inc"

.IMPORT _spdcadr2
.IMPORT _mountaincol1, _mountaincol3
;Import from colorfade.s
.IMPORT bergcolordata, bergdata
;Import from InitMountains.s
 
.EXPORT txtscrll
txtscrll:
  .EXPORT xpos
          dec xpos
          dec xpos
          lda xpos
          cmp #$ff
          beq scrollleftonechar
          rts
xpos
 .byte 3
;---------------------------------------          
scrollleftonechar
          lda #7
          sta xpos

          ldx #0
rleft:
	.REPEAT mountainheight, row
          lda bergscrladr+1+(row*SCRW),x
          sta bergscrladr+(row*SCRW),x
  .ENDREP
  				inx
          cpx #SCRW-1
          bne rleft
          
          ldy bergxpos
          jsr fademountains
  .REPEAT mountainheight, row
          lda bergdata+row*SCRW,y
          sta bergscrladr+(row+1)*SCRW-1
 	.ENDREP
 	        iny
          cpy #mountainxsize
          bne *+4
          ldy #0
          sty bergxpos
          rts
bergxpos
 .byte 0
curxpos:
	.BYTE 0
fademountains:
					jmp (_spdcadr2)
;Trick, to imitate jsr(adress) instead of jsr $ffff and 
;selfmodification

