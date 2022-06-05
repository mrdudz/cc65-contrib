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
		.include "lynx.inc"
	.include "../resident/extzp.inc"
		.import     FileReadBytes, FileReadOver, FileSelectBlock
		.import     FileLoadDir
		.export     FileLoadFile
		.export     _FileLoadFile

		.import     popa, popax, tstax


		.code

;**********************************
; Open file, i.e. select block, over-read offset, set DestPtr
; In: Accu = file number
; Out: X:Y = file length
;**********************************
FileOpenFile:   jsr FileLoadDir
		lda _FileDestAddr
		ora _FileDestAddr+1      ; dest == 0?
		bne cont0              ; no =>

		lda _FileDestPtr
		sta _FileDestAddr
		lda _FileDestPtr+1
		sta _FileDestAddr+1
		bra cont1

cont0:
		lda _FileDestAddr
		sta _FileDestPtr
		lda _FileDestAddr+1
		sta _FileDestPtr+1

cont1:          lda _FileStartBlock
		sta _FileCurrBlock       ; set startblock
		jsr FileSelectBlock

		ldx _FileBlockOffset
		ldy _FileBlockOffset+1   ; set offset
		jsr FileReadOver

		ldx _FileFileLen
		ldy _FileFileLen+1       ; return file length
		rts

;**********************************
; load file
; In: Accu = file number
;**********************************
_FileLoadFile:
FileLoadFile:   jsr FileOpenFile        ; set position in cartridge and
					; get file length in X:Y
		jsr FileReadBytes       ; read the whole file to its DestAddr
		ldx _FileDestAddr        ; return destination address
		ldy _FileDestAddr+1
		rts
