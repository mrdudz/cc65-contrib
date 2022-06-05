.EXPORT _mainloop

.IMPORT _copyclowds

.PROC _mainloop
	jsr _copyclowds
	
	lda #$01
:	bit $dd0d
	beq :-
	
	rts
.ENDPROC