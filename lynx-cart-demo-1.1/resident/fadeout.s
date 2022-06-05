; ***
; CC65 Lynx Library
;
; Shawn Jefferson, June 2004
; 
; ***
;
; void __fastcall__ fade_out(unsigned char interval)
;
; Fade the palette out from the current palette values in hardware
; to an all black palette, waiting (interval * 64us)*2 between palette
; changes.
;
; This routine uses timer7 to time the interval between each palette change.
; Make sure you aren't using timer7 for some other purpose when you call
; this function.  We don't retain use of timer7 after exit.
;

		.include    "lynx.inc"
	.include "../resident/extzp.inc"
		.export     _fade_out
		.importzp   tmp1

INTMULT =   5                       ; interval multiplier

		.code

_fade_out:      sta TIMER7           ; timer backup value = interval
		sta TIMER7+2         ; count = interval
		lda #$1E            ; reload, count, 64us
		sta TIMER7+1         ; timer control value
		stz TIMER7+3         ; dynamic control = 0

again:          ldy #15             ; 16 colors in pallette
loop:           lda PALETTE,y
		beq redblue
		dea                 ; decrement rgb value
		sta PALETTE,y 
redblue:        tya
		ora #16             ; index into redblue values
		tay
		lda PALETTE,y
		and #$F0            ; compare red
		beq chkblue
		lda PALETTE,y
		sec
		sbc #$10            ; decrement rgb value
		sta PALETTE,y
chkblue:        lda PALETTE,y
		and #$0F            ; compare blue
		beq cont
		lda PALETTE,y
		dea                 ; decrement rgb value
		sta PALETTE,y

cont:           tya                 ; next palette index
		and #15             ; back to green index
		tay
		dey 
		bpl loop

		ldy #31             ; check palettes same?
checkloop:      lda PALETTE,y
		bne wait            ; run through palette again 
		dey
		bpl checkloop
		bra done

wait:           ldy #INTMULT        ; (64us * interval) * INTMULT
waitagain:      lda TIMER7           ; load the backup value
		sta TIMER7+2         ; reset count with backup value
interval:       lda TIMER7+2         ; wait the interval
		bne interval
		dey 
		bne waitagain
		bra again           ; run through palette again

done:           lda #$00            ; timer off
		sta TIMER7+1
		rts
