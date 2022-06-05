; Atari Lynx title sprite shown at boot time.
; Frankly I have no idea of how to build one yet.
; So I have to use INSERT.O It is somehow hardcoded
; to go to $2400. So __TITLE_START__ has to be
; $2400.
;
; modified from newcc65 by Karri Kaksonen, 2004
;

.segment	"TITLESPR"
palette:
	.byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	.byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
sprite:
	.byte $01,$10,$20
	.addr 0, sprite
	.word 0,0
	.word $100,$100
	.byte $0F
	.byte	3,%10000100,%00000000, $0
