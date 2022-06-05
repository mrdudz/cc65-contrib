.EXPORT vicbank:abs
.EXPORTZP _vicbankbits
.EXPORT _ghostbyte
vicbank = $0000
_vicbankbits = 3-(vicbank/$4000)
_ghostbyte=vicbank+$3fff
	;these constants are needed in the c programm, so i must 
	;export them. i can't use the assembler include file in the
	;c-subroutine.
