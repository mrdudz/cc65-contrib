_FADECODESNIBBLE:
	sec
_fademapelement = *+1
	sbc #$ff
	tax
	lda coltab,x
_fadescradr = *+1
	sta $ffff
	tya
_FADECODELEN = (*-_FADECODESNIBBLE)

coltab: .BYTE $00,$60,$b6,$4b,$c4,$ec,$3c,$fc
				.RES 128-(*-coltab),$fc
				.RES 256-(*-coltab),0
				
.EXPORT _FADECODESNIBBLE, _fademapelement, _fadescradr
.EXPORTZP _FADECODELEN