;Caverns of the lost miner version C
;2007 (LGPL) BAKTRA 

;Assembler routines. I like CC65, because it's easy to add assembler routines
;to a C program. And I also like KDE's editor component, because 6502 assembler
;syntax highlighting information is bundled. Under Microsoft Windows, I recomend
;PSPad programmer's editor (www.pspad.com). It is great freeware editor.

;If you make modifications and _vbistorel and _vbistoreh will be at different memory
;pages, align them using some "padding" bytes




;Supplementary variables
.segment "CODE"
_sfx:		.byte 0
_vbistorel:	.byte 0
_vbistoreh:	.byte 0
_suspend:	.byte 0
_mvDelay:	.byte 0

.segment "CODE"
_dliHandler:
	pha
	sta 54282	;Horiz retrace
	lda #4		;DARK BG
	sta 53272
	lda #12
	sta 53271	;BRIGHT FONT
	lda #<_dliHandler2
	sta 512
	lda #>_dliHandler2
	sta 513
	pla
	rti
_dliHandler2:
	pha
	sta 54282	;Horiz retrace
	;Restore colors, swap after DLI
	lda #134		;BLACK BG
	sta 53272
	lda #12
	sta 53271
	lda #<_dliHandler
	sta 512
	lda #>_dliHandler
	sta 513
	pla
	rti

;Here is VBI CALLBACK
.segment "CODE"
_vbiRoutine:
	php
	pla
	;movement delay
	lda _mvDelay
	cmp #0
	beq _n
	dec _mvDelay
	
	;if audio is suspended, do not call RMT routines
_n:	lda _suspend
	cmp #0
	bne _x1
	;if SFX not requested, continue to music update
	lda _sfx
	cmp #0
	beq _x
	;SFX
	ldx #3
	lda #30
	ldy _sfx
	jsr $200f
	lda #0
	sta _sfx
	;Music update - this is like MikMod :-)	
_x:		jsr $2003	

	
_x1:	jmp (_vbistorel)


;Setting up a VBI routine ---------------------------
.segment "CODE"

.proc _rmtSetVBI: near
.segment "CODE"
	;store original vbi address
	lda 548
	sta _vbistorel
	lda 549
	sta _vbistoreh
	;Set new 
	ldx #>_vbiRoutine
	ldy #<_vbiRoutine
	lda #7
	jsr $e45c
	
	rts
.endproc

;Restore VBI original routine
.segment "CODE"

.proc _rmtRestoreVBI: near
.segment "CODE"
	;store original vbi address
	
	ldy _vbistorel
	ldx _vbistoreh
	lda #7
	jsr $e45c
	
	rts
.endproc

;-Initialize RASTER music tracker------------------
.segment "CODE"

.proc _rmtInitMenuMusic: near
.segment "CODE"
	;Music file is at page 100
	ldx #0
	ldy #100
	;First song line
	lda #0
	;Initialize the tracker
	jsr $2000
	;End of procedure
	rts
.endproc

.proc _rmtInitGameMusic: near
.segment "CODE"
	;Music file is at page 100
	ldx #0
	ldy #100
	;Tenth song line
	lda #10
	;Initialize the tracker
	jsr $2000
	;End of procedure
	rts
.endproc

;-Stop RMT routine
.segment "CODE"

.proc _rmtAllStop: near
.segment "CODE"
	jsr $2006
	rts
.endproc



;-Play diamond picked sound------------
.segment "CODE"

.proc _rmtPlayDiamond :  near
.segment "CODE"
	lda #10
	sta _sfx
	rts
.endproc

;-Play jump sound------------
.segment "CODE"

.proc _rmtPlayJump :  near
.segment "CODE"
	lda #18
	sta _sfx
	rts
.endproc

;-Play congratulations sound ---------------
.segment "CODE"

.proc _rmtPlayGratulation :  near
.segment "CODE"
	lda #16
	sta _sfx
	rts
.endproc

;-Play death sound-----------------------
.segment "CODE"

.proc _rmtPlayDeath : near
.segment "CODE"
	lda #14
	sta _sfx
	rts
.endproc

;-Play all diamonds collected sound
.segment "CODE"

.proc _rmtPlayPicked : near
.segment "CODE"
	lda #12
	sta _sfx
	rts
.endproc

;-Suspend RMT routine--------
.segment "CODE"

.proc _rmtSuspend : near
.segment "CODE"
	lda #1
	sta _suspend
	rts
.endproc

;-Resume RMT routine--------
.segment "CODE"

.proc _rmtResume : near
.segment "CODE"
	lda #0
	sta _suspend
	rts
.endproc

;-OS call - cold start--------
.segment "CODE"

.proc _asmReboot : near
.segment "CODE"
	jmp 58487
.endproc

.export _mvDelay
.export _rmtSuspend
.export _rmtResume
.export _rmtPlayPicked
.export _rmtPlayDeath
.export _rmtPlayDiamond
.export _rmtPlayGratulation
.export _rmtInitMenuMusic
.export _rmtInitGameMusic
.export _rmtSetVBI
.export _rmtAllStop
.export _rmtPlayJump
.export _rmtRestoreVBI
.export _asmReboot
.export _dliHandler
