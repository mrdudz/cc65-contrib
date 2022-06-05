.EXPORT _waitretrace, _waitrasterline

_waitretrace:
:	lda $d011
	bpl :-
:	lda $d011
	bmi :-
	rts

_waitrasterline:
:	cmp $d012
	bne :-
	rts