; ***
; CC65 Lynx Library
;
; Originally by Bastian Schick 
; http://www.geocities.com/SiliconValley/Byte/4242/lynx/
;
; Ported to cc65 (http://www.cc65.org) by
; Shawn Jefferson, June 2004
; 
; ***
; 
; void init_irq(void)
;
; Initialize the IRQ handler.  You must call this before you call install_irq!
;
; ***
;
; void install_irq(unsigned char num, int (* func)() );
;
; Set an function to be called by the timer interrupt, num.  The interrupt
; doesn't take effect until it is enabled (use the ENABLE_IRQ() macro.)
;
; ***
;
; void deinstall_irq(unsigned char num);
;
; Removes the interrupt function pointer from the handler for timer interrupt,
; num.
;


	.include    "lynx.inc"
	.include "../resident/extzp.inc"
	.export     _init_irq, _install_irq, _deinstall_irq
	.import     popax, popa, decsp2 


	.data

__IRQ:          phy
		phx
		pha
		cld             ; binary mode

		lda INTSET      ; get pending interrupt(s)
		sta __IRQ1+1
		lda #0          ; start checking with HBL
		tax
		tay             ; no parameters
		ina
__IRQ1:         bit #00
		bne __IRQ2
		inx
		inx
		asl A 
		bne __IRQ1
		bra __IRQ3

__IRQ2:         sta INTRST      ; clear interrupt
		jsr __IRQ5      ; call ISR

__IRQ3:         pla
		plx
		ply
		rti

__IRQ5:         jmp (_irqvecs,x)

DummyIRQ:
_init_irq:      php             ; will be replace by RTS !
saveIRQ:        sei
_irqvecs:
;
; we use this space later for the vectors
; so InitIRQ is disabled
;
		lda #$60        ; opcode for RTS
		sta _init_irq   ; disable _InitIRQ

		lda #<__IRQ     ; set vector
		ldx #>__IRQ     ;     |
		sta INTVECTL    ; $FFFE
		stx INTVECTH    ; $FFFF

		ldy #15 ; init. dispatcher
		ldx #<DummyIRQ
_InitIRQ1: 
		lda #>DummyIRQ
		sta _irqvecs,y
		dey
		txa
		sta _irqvecs,y
		dey
		bpl _InitIRQ1
		plp
_InitIRQ2:      rts


; install_irq

_install_irq:
		php
		sei
		jsr popax       ; get funptr   FASTCALL -SJ
		phx
		pha
		jsr popa        ; get num
		asl A                           ; * 2
		tay
		pla                             ; pop low-byte funptr
		sta _irqvecs,y
		pla                             ; pop high-byte
		sta _irqvecs+1,y
		plp
		rts

;
; C: DeInstallIRQ(int num)
;

_deinstall_irq:
		php
		sei
		jsr popa          ; get num  FASTCALL -SJ
		asl A
		tay
		lda #<DummyIRQ
		sta _irqvecs,y
		lda #>DummyIRQ
		sta _irqvecs+1,y
		plp
		rts
