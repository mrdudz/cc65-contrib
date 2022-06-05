.FEATURE labels_without_colons

RASTERSPEEDRESET = 15

.SEGMENT "BARCODE"
.EXPORT showrbar
.PROC showrbar
	.IMPORTZP suncol
          ldy rasterpos
make0
          ldx #18
make
          lda farbtab,y
          sta $d021 ;schw.-rot-schw.-cyan-schw.
          lda waittab,x
          jsr waitnand14cyc
          nop
          iny
          dex
          bne make
          
          dec rasterspeed
          bpl norasterfade
          
          lda #RASTERSPEEDRESET
          sta rasterspeed
          
          ldx rasterpos
          cpx #rasterlen
          beq norasterfade
          inc rasterpos
norasterfade:          
          rts
farbtab
;because i use 8 sprites for the mountains, i can't split $d021 in a badline.
;every badline and the next one have the same color. this is the only solution
;the first 6 lines are no badlines and have different colors. then the next
;two are badlines and have the same color.
;after that 8 lines are complete, so start again.
.IF ::DEBUG = 0
 .byte 14,14,14,14,14,14;light red = two times, cause of badline
 .BYTE 14,14,14,14,14,14;yellow = two times, "
 .byte 14,14,14,14,14,14,14
rasterlen = *-farbtab
  .byte 3,14,3,3,15,3;light red = two times, cause of badline
 .BYTE 15,15,13,15,13,13;yellow = two times, "
 .byte 1,13,1,1,1,1,1
.ELSE
 .byte 1,0,2,0,3,0,4,0,5,0,6,0,7,0,8,0,9,0,10
rasterlen = *-farbtab
 .BYTE 1,0,2,0,3,0,4,0,5,0,6,0,7,0,8,0,9,0,10
.ENDIF
waittab:
	.BYTE 22,22,22,22,21,22,22,22, 22,22,22,22,21,22,22,22 
	.BYTE 22,22,22,22,22,22,21,22, 22,22,22,22,22,22,22,22
rasterpos:
.BYTE 0
rasterspeed:
.BYTE RASTERSPEEDRESET
;---------------------------------------
waitnand14cyc
					sta wmod+1
					clc
wmod:
					bcc *+2
					cmp #$c9
					cmp #$c9
					cmp #$c9
					cmp #$c9
					cmp #$c9
					cmp #$c9
					cmp #$c9
					cmp #$c9
					cmp #$c9
					cmp #$c9
					bit $ea
					nop
          rts
;---------------------------------------
.ENDPROC