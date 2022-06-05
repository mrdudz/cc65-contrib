
.include "c64.inc"

.include "cbm.i"
.include "opcodes.i"
.include "zpvar.i"
.include "stack.i"
.include "stdscr.i"
.include "16bit.mac"

COLOR_RAM = $d800

.IMPORT initsqr,nextsqr,sqrt16
.IMPORT _waitvsync

.DEFINE CHARSET_LOC $4000

.DEFINE CYCTAB_LOC $4200
.DEFINE COLTAB_LOC $4400

.DEFINE SCRN_LOC $4800

.DEFINE SPEEDCODE_ADDR $5400

COLOR00 = $00
COLOR01 = $06   ; bg
COLOR02 = $02
COLOR03 = $04   ; bg
COLOR04 = $0e
COLOR05 = $05   ; bg
COLOR06 = $03
COLOR07 = $07   ; bg
COLOR08 = $01

.DEFINE DD00VAL $c6
.DEFINE D018VAL $20
.DEFINE D011VAL $5b
.DEFINE D020VAL $00

;.DEFINE D021VAL $0b
;.DEFINE D022VAL $08
;.DEFINE D023VAL $05
;.DEFINE D024VAL $07

D021VAL = COLOR01
D022VAL = COLOR03
D023VAL = COLOR05
D024VAL = COLOR07

.DEFINE CYCX 0
.DEFINE CYCY 0
.DEFINE CYCH 12
.DEFINE CYCW 20
.DEFINE CYCADR CYCY*SCRW+CYCX+CYCW-1+SCRN_LOC
.DEFINE CYCADR2 CYCH*SCRW+CYCADR

z	= 2;global pointers
z2 = 4

localzp = $20;z_localzp

	.segment "INIT"
	.segment "ONCE"
	.segment "STARTUP"
	.segment "CODE"
;.ADDR *+2
_main:
	sei
	
	ldy #0
	ldx #0
:
	lda CHARSET,x
	sta CHARSET_LOC+0,y
	inx
	iny
	bne :-
:
	lda CHARSET,x
	sta CHARSET_LOC+$100,y
	dex
	iny
	bne :-
	
:
	lda CHARSET_LOC+$000,x
	sta CHARSET_LOC+$200,y
	sta CHARSET_LOC+$400,y
	sta CHARSET_LOC+$600,y
	lda CHARSET_LOC+$100,x
	sta CHARSET_LOC+$300,y
	sta CHARSET_LOC+$500,y
	sta CHARSET_LOC+$700,y
	inx
	iny
	bne :-

	ldx #0
:
	lda CYCTAB,x
	sta CYCTAB_LOC,x
	lda CYCTAB+$100,x
	sta CYCTAB_LOC+$100,x
	lda COLTAB,x
	sta COLTAB_LOC,x
	lda COLTAB+$100,x
	sta COLTAB_LOC+$100,x
	inx
	bne :-

	ldx #0
:
	lda #$00
	sta $d800,x
	sta $d900,x
	sta $da00,x
	sta $db00,x
	lda #$80
	sta SCRN_LOC,x
	sta SCRN_LOC+$100,x
	sta SCRN_LOC+$200,x
	sta SCRN_LOC+$300,x
	inx
	bne :-
	
;	jsr initsqr
	jsr initpart

	ldx #CYCW
	ldy #CYCH
	lda #<(SPEEDCODE_ADDR)
	pha
	lda #>(SPEEDCODE_ADDR)
	pha
	lda #<(CYCADR2-SCRW)
	pha
	lda #>(CYCADR2-SCRW)
	pha
	lda #<(CYCADR2)
	pha
	lda #>(CYCADR2)
	jsr prepcycling
	pla
	pla
	pla
	pla
	pla
	jmp makecycling
	rts
;-------------------------------------
; main loop
.PROC makecycling
	jsr _waitvsync
	inc $d020
cycpos = *+1
	ldx #0
	jsr SPEEDCODE_ADDR
	dec $d020

	inc cycpos
	
	jmp makecycling
.ENDPROC
;-------------------------------------
; prepare speedcode
.PROC prepcycling
xpos = localzp
ypos = localzp+1
ysqrcnt = localzp+2
ysqr = localzp+3
xsize = localzp+5

	stx xpos
	stx xsize
	sty ypos

	sta cycdest1+1
	tay
	and #3
	ora #>COLOR_RAM
	sta coldest1+1
	
	tsx
	lda STACK+3,x
	sta cycdest1
	sta coldest1
	clc
	adc #1
	sta cycdest3
	sta coldest3
	tya
	adc #0
	sta cycdest3+1
	and #3
	ora #>COLOR_RAM
	sta coldest3+1

	lda STACK+5,x
	sta cycdest2
	sta coldest2
	clc
	adc #1
	sta cycdest4
	sta coldest4
	lda STACK+4,x
	sta cycdest2+1
	pha
	adc #0
	sta cycdest4+1
	and #3
	ora #>COLOR_RAM
	sta coldest4+1
	pla
	and #3
	ora #>COLOR_RAM
	sta coldest2+1

	lda STACK+6,x
	sta z+1
	lda STACK+7,x
	sta z

	sec
	jsr initsqr
	stx ysqr
	sty ysqr+1
	sta ysqrcnt

yloop:
	ldx xsize
	stx xpos

	sec
	jsr initsqr
xloop:
	txa
	clc
	adc ysqr
	pha
	tya
	adc ysqr+1
	tax
	pla
	jsr sqrt16
	sta cycphase
	sta colphase
	
	ldy #spdce-spdc-1
:	lda spdc,y
	sta (z),y
	dey
	bpl :-

	dec16 cycdest1
	dec16 cycdest2
	dec16 coldest1
	dec16 coldest2
	inc16 cycdest3
	inc16 cycdest4
	inc16 coldest3
	inc16 coldest4

	;lda #spdce-spdc
	;ldx #z
	;jsr addzp		; FIXME? wth?
	lda z
	clc
	adc #(spdce-spdc)
	sta z
	bcc :+
	inc z+1
:
	jsr nextsqr
	dec xpos
	bne xloop

	lda cycdest1
	clc
	adc #SCRW+(CYCW)
	sta cycdest1
	sta coldest1
	bcc :+
	inc cycdest1+1
	inc coldest1+1
:
	lda cycdest3
	clc
	adc #SCRW-(CYCW)
	sta cycdest3
	sta coldest3
	bcc :+
	inc cycdest3+1
	inc coldest3+1
:
	lda cycdest2
	sec
	sbc #SCRW-(CYCW)
	sta cycdest2
	sta coldest2
	bcs :+
	dec cycdest2+1
	dec coldest2+1
:
	lda cycdest4
	sec
	sbc #SCRW+(CYCW)
	sta cycdest4
	sta coldest4
	bcs :+
	dec cycdest4+1
	dec coldest4+1
:
	ldx ysqr
	ldy ysqr+1
	lda ysqrcnt
	clc
	jsr initsqr
	jsr nextsqr
	stx ysqr
	sty ysqr+1
	sta ysqrcnt

	dec ypos
	beq :+
	jmp yloop
:
	ldy #0
	lda #RTSCODE
	sta (z),y
	rts
spdc:	
cycphase=*+1
			lda CYCTAB_LOC,x
cycdest1=*+1
			sta $ffff
cycdest2=*+1
			sta $ffff
cycdest3=*+1
			sta $ffff
cycdest4=*+1
			sta $ffff
colphase=*+1
			lda COLTAB_LOC,x
coldest1=*+1
			sta $ffff
coldest2=*+1
			sta $ffff
coldest3=*+1
			sta $ffff
coldest4=*+1
			sta $ffff
spdce:
.ENDPROC
;-------------------------------------
.PROC initpart
	lda #DD00VAL
	sta $dd00
	lda #D018VAL
	sta $d018
	lda #D011VAL
	sta $d011
	lda #D020VAL
	sta $d020
	lda #D021VAL
	sta $d021
	lda #D022VAL
	sta $d022
	lda #D023VAL
	sta $d023
	lda #D024VAL
	sta $d024
	rts
.ENDPROC

;------------------------------------------------------------------------------

CHARSET: ; $4000-
;	.incbin "colcyc-data.bin",$000,$100
	.byte %11111111
	.byte %11111111
	.byte %11111111
	.byte %11111111
	.byte %11111111
	.byte %11111111
	.byte %11111111
	.byte %11111111
	
	.byte %11111111
	.byte %10111011
	.byte %11111111
	.byte %11111111
	.byte %11111111
	.byte %10111011
	.byte %11111111
	.byte %11111111
	
	.byte %11111111
	.byte %10111011
	.byte %11111111
	.byte %11101110
	.byte %11111111
	.byte %10111011
	.byte %11111111
	.byte %11101111
	
	.byte %11111111
	.byte %01010101
	.byte %11111111
	.byte %11101110
	.byte %11111111
	.byte %01010101
	.byte %11111111
	.byte %10111011
	
	.byte %11111111
	.byte %01010101
	.byte %11111111
	.byte %10101010
	.byte %11111111
	.byte %01010101
	.byte %11111111
	.byte %10101010
	
	.byte %11101110
	.byte %01010101
	.byte %10111011
	.byte %01010101
	.byte %11101110
	.byte %01010101
	.byte %10111011
	.byte %01010101
	
	.byte %10101010
	.byte %01010101
	.byte %10111011
	.byte %01010101
	.byte %10101010
	.byte %01010101
	.byte %10111011
	.byte %01010101
	
	.byte %10101010
	.byte %01010101
	.byte %10101010
	.byte %01010101
	.byte %10101010
	.byte %01010101
	.byte %10101010
	.byte %01010101
	; $08
	.byte %10101010
	.byte %01010101
	.byte %00100010
	.byte %01010101
	.byte %10101010
	.byte %01010101
	.byte %00100010
	.byte %01010101
	
	.byte %10001000
	.byte %01010101
	.byte %00100010
	.byte %01010101
	.byte %10001000
	.byte %01010101
	.byte %00100010
	.byte %01010101
	
	.byte %10001000
	.byte %01010101
	.byte %00100010
	.byte %01000101
	.byte %10001000
	.byte %01010100
	.byte %00100010
	.byte %01000101
	
	.byte %01010101
	.byte %00000000
	.byte %10101010
	.byte %00000000
	.byte %01010101
	.byte %00000000
	.byte %10101010
	.byte %00000000
	
	.byte %01010101
	.byte %00000000
	.byte %10001000
	.byte %00000000
	.byte %01010101
	.byte %00000000
	.byte %00100010
	.byte %00000000
	
	.byte %01000100
	.byte %00000000
	.byte %00010001
	.byte %00000000
	.byte %01000100
	.byte %00000000
	.byte %00010000
	.byte %00000000
	
	.byte %01000100
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %01000100
	.byte %00000000
	.byte %00000000
	.byte %00000000
	; $0f
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	.byte %00000000
	
	.res $80 * 8, 0

CYCTAB:  ; $4100-
.repeat 2, m
	; 8 times repeated:
	; X0..Xf, Xf..X00
	; with X being 0,4,8,c c,8,4,0
	.repeat 16, n
	.byte $00 + n
	.endrepeat
	.repeat 16, n
	.byte $00 + (15 - n)
	.endrepeat
	.repeat 16, n
	.byte $40 + n
	.endrepeat
	.repeat 16, n
	.byte $40 + (15 - n)
	.endrepeat
	.repeat 16, n
	.byte $80 + n
	.endrepeat
	.repeat 16, n
	.byte $80 + (15 - n)
	.endrepeat
	.repeat 16, n
	.byte $c0 + n
	.endrepeat
	.repeat 16, n
	.byte $c0 + (15 - n)
	.endrepeat
	.repeat 16, n
	.byte $c0 + n
	.endrepeat
	.repeat 16, n
	.byte $c0 + (15 - n)
	.endrepeat
	.repeat 16, n
	.byte $80 + n
	.endrepeat
	.repeat 16, n
	.byte $80 + (15 - n)
	.endrepeat
	.repeat 16, n
	.byte $40 + n
	.endrepeat
	.repeat 16, n
	.byte $40 + (15 - n)
	.endrepeat
	.repeat 16, n
	.byte $00 + n
	.endrepeat
	.repeat 16, n
	.byte $00 + (15 - n)
	.endrepeat
.endrepeat

; color 1/3/5/7 are bg
COLTAB:  ; $4300-
.repeat 2

; $00 full col (=0)               d021 = $06
;.repeat 16
;.byte COLOR00
;.endrepeat
.repeat 16
.byte COLOR00
.endrepeat
; $20 full bg  (=6)
.repeat 16
.byte COLOR02
.endrepeat
.repeat 16
.byte COLOR02
.endrepeat
; $30 full col (=2)

; $40 full col (=c)               d022 = $04
.repeat 16
.byte COLOR02
.endrepeat
.repeat 16
.byte COLOR02
.endrepeat

.repeat 16
.byte COLOR04
.endrepeat
.repeat 16
.byte COLOR04
.endrepeat


; $80 full col (=1)               d023 = $05

.repeat 16
.byte COLOR05
.endrepeat
.repeat 16
.byte COLOR05
.endrepeat

.repeat 16
.byte COLOR06
.endrepeat
.repeat 16
.byte COLOR06
.endrepeat

; $c0 full col (=c)               d024 = $07

.repeat 16
.byte COLOR07
.endrepeat
.repeat 16
.byte COLOR07
.endrepeat

.repeat 16
.byte COLOR07
.endrepeat
.repeat 16
.byte COLOR07
.endrepeat

.repeat 16
.byte COLOR00
.endrepeat

.endrepeat

.repeat 2

; $00 full col (=9)               d021 = $0b
.byte $09 ,$09 ,$09 ,$09 ,$09 ,$09 ,$09 ,$09
.byte $09 ,$09 ,$09 ,$09 ,$09 ,$09 ,$09 ,$09
; $10 full bg  (=b)
.byte $02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02
.byte $02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02
; $20 full col (=2)
.byte $02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02
.byte $02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02
; $30 full bg  (=b)
.byte $0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C
.byte $0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C

; $40 full col (=c)               d022 = $08
.byte $0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C
.byte $0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C
; $50 full bg  (=8)
.byte $0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F
.byte $0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F
; $60 full col (=f)
.byte $0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F
.byte $0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F
; $70 full bg  (=8)
.byte $01 ,$01 ,$01 ,$01 ,$01 ,$01 ,$01 ,$01
.byte $01 ,$01 ,$01 ,$01 ,$01 ,$01 ,$01 ,$01

; $80 full col (=1)               d023 = $05
.byte $01 ,$01 ,$01 ,$01 ,$01 ,$01 ,$01 ,$01
.byte $01 ,$01 ,$01 ,$01 ,$01 ,$01 ,$01 ,$01
; $90 full bg  (=5)
.byte $0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F
.byte $0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F
; $a0 full col (=f)
.byte $0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F
.byte $0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F ,$0F
; $b0 full bg  (=5)
.byte $0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C
.byte $0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C

; $c0 full col (=c)               d024 = $07
.byte $0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C
.byte $0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C ,$0C
; $d0 full bg  (=7)
.byte $02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02
.byte $02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02
; $e0 full col (=2)
.byte $02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02
.byte $02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02 ,$02
; $e0 full bg  (=7)
.byte $09 ,$09 ,$09 ,$09 ,$09 ,$09 ,$09 ,$09
.byte $09 ,$09 ,$09 ,$09 ,$09 ,$09 ,$09 ,$09

.endrepeat
