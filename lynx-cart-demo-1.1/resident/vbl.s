;
; File generated by cc65 v 2.10.5
;
	.fopt		compiler,"cc65 v 2.10.5"
	.setcpu		"65C02"
	.smart		on
	.autoimport	on
	.case		on
	.debuginfo	off
	.importzp	sp, sreg, regsave, regbank, tmp1, ptr1, ptr2
	.macpack	longbranch
	.import		_tgi_setviewpage
	.import		_tgi_setdrawpage
	.export		_drawPending
	.export		_metronome
	.export		_drawpage
	.export		_vbl
	.export		_waitsuzy

.segment	"DATA"

_drawPending:
	.byte	$00
_metronome:
	.word	$0000
_drawpage:
	.byte	$00

; ---------------------------------------------------------------
; void __near__ vbl (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_vbl: near

.segment	"CODE"

	pha
	phx
	phy
	lda     _drawPending
	beq     @L1
	lda     _drawpage
	jsr     _tgi_setviewpage
	lda     _drawpage
	eor	#$01
	sta     _drawpage
	jsr     _tgi_setdrawpage
	stz     _drawPending
@L1:	inc     _metronome
	bne     @L2
	inc     _metronome+1
@L2:	ply
	plx
	pla
	rts

.endproc

; ---------------------------------------------------------------
; void __near__ waitsuzy (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_waitsuzy: near

.segment	"CODE"

@L1:	bit	$fc92
	bmi	@L1
	rts

.endproc

