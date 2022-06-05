		.include "lynx.inc"
	.include "../resident/extzp.inc"
		.export     _FileExecFile
		.import     FileLoadFile
		.code

;**********************************
; exec file
; In: Accu = file number
;**********************************
_FileExecFile:  jsr FileLoadFile        ; set position in cartridge and
		stx _FileDestPtr
		sty _FileDestPtr+1
	        jmp (_FileDestPtr)
