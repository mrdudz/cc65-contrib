.EXPORT _spdcdef, _srcadr, _destadr

testspeedcode:
_srcadr=*+1
  lda $0400
_destadr=*+1
	sta $03ff
testspeedcodeend:


spdcdef:
_spdcdef:
	.WORD testspeedcode
	.BYTE testspeedcodeend-testspeedcode
