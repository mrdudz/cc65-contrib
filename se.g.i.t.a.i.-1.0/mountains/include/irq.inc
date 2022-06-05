.IFNDEF __IRQ_INC
__IRQ_INC:
	.INCLUDE "clowds.inc"
	.INCLUDE "mountains.inc"

	bergirqlin=clowdspry+9;
	upperirqline=$37
	boffirqlin=mountainy*8+$32+21;231
	
.ENDIF