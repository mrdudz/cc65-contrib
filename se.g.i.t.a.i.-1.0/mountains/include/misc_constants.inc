.IFNDEF __MISCCONST_INC
__MISCCONST_INC:
	.IMPORT __BERGCHAR_LOAD__, __SCREEN1_LOAD__, __SCREEN2_LOAD__
	.IMPORT vicbank
	
	sprdefptr1=__SCREEN1_LOAD__+$3f8
	sprdefptr2=__SCREEN2_LOAD__+$3f8
	char=$3000
	scrnofs=__SCREEN1_LOAD__-vicbank
	scrnofs2=__SCREEN2_LOAD__-vicbank
	charofs=__BERGCHAR_LOAD__-vicbank
	
	vicadr=<( (scrnofs/$40)+(charofs/$0400) )
	vicadr2=<( (scrnofs2/$40)+(charofs/$0400) )
.ENDIF