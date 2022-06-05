.EXPORT _rasterirq, _rti

.IMPORT __ZP_START__, __ZP_SIZE__
;defined in linker config file

.IMPORT pushax, pusha
;software stack functions of runtime library

.IMPORT _logofader, _textfader, _logomoves, _quit
.IMPORT _getspacestat

.IMPORT _music_play, _music_fader 

.IMPORT _switch_raster_irq, _setsplitirq 

_rasterirq:
	lsr $d019
	
	jsr savezp
		
	jsr _logofader
	tax
	beq changeirq 
	
	lda #250
: cmp $d012
	bne :-
	
	lda $d011
	and #$f0
	ora #$0b
	sta $d011
	
	jsr _music_play
	jmp :+
changeirq:
	lda #250
	ldx #0
	jsr pushax
	lda #<rasterirq2
	ldx #>rasterirq2
	jsr _switch_raster_irq
	
	jsr _logofader
	sta logopos
:	jsr _music_fader
	jsr _getspacestat
	bne :+

	lda #1
	sta _quit
:
	jsr restorezp
	
	pla
	tay
	pla
	tax
	pla
_rti:	rti
;important for irqinit to disable nmi
;don't use jmp $ea81 here!!!
;--------------------------------
rasterirq2:
	lsr $d019
	
	jsr savezp
	
	lda $d011
	and #$f0
	ora #$0b
	sta $d011
	
	ldx #$1d
	lda logopos
	bne :+
	ldx #$08
:	stx $d016
	
	jsr _music_play
	jsr _music_fader
	jsr _logomoves
	tax
	beq noirqchange
	
	lda #$1d
	sta $d016
	
	lda #25
	ldx #0
	jsr pushax
	lda #<_rasterirq
	ldx #>_rasterirq
	jsr _switch_raster_irq
	
	jsr restorezp

	jsr $ffea ;increase timers.Important!!!
	jmp $ea81
noirqchange:		
	lda logopos
	jsr pusha
	lda #<splitirq
	ldx #>splitirq
	jsr _setsplitirq
	
	jsr _getspacestat	
	bne :+

	lda #1
	sta _quit
:
	cli
	jsr _textfader
	jsr restorezp
	jsr $ffea ;look before
	jmp $ea81 
;--------------------------------
splitirq:
	lsr $d019
	
	ldx #$1d
	lda logopos
	beq :+
	ldx #$08
:	stx $d016

	lda #250
	ldx #0
	jsr pushax
	lda #<rasterirq2
	ldx #>rasterirq2
	jsr _switch_raster_irq
	
	jmp $ea81
;--------------------------------
logopos:
.RES 1
;--------------------------------
.BSS
zptemp:
	.RES $1a
.CODE

savezp:
	.REPEAT $1a, zpoffset
		lda __ZP_START__+zpoffset
		sta zptemp+zpoffset
	.ENDREP
;__ZP_SIZE__ = 0x1a, but not defined at compile time->problem
	rts
restorezp:
	.REPEAT $1a, zpoffset
		lda zptemp+zpoffset
		sta __ZP_START__+zpoffset
	.ENDREP
;__ZP_SIZE__ = 0x1a, but not defined at compile time->problem
	rts 