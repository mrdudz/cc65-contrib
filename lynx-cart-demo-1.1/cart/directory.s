; The Atari Lynx directory structure
; Written for the cc65 compiler by
; Karri Kaksonen, 2004
;
	.import __STARTUP_LOAD__
	.import __STARTUP_SIZE__
	.import __INIT_SIZE__
	.import __CODE_LOAD__
	.import __CODE_SIZE__
	.import __RODATA_SIZE__
	.import __DATA_SIZE__
	.import __LOADER_SIZE__
	.import __TITLE_START__
	.import __TITLE_SIZE__
	.import __INTRO_CODE_LOAD__
	.import __INTRO_CODE_SIZE__
	.import __INTRO_DATA_SIZE__
	.import __INTRO_RODATA_SIZE__
	.import __SKETCH_CODE_LOAD__
	.import __SKETCH_CODE_SIZE__
	.import __SKETCH_DATA_SIZE__
	.import __SKETCH_RODATA_SIZE__

.segment	"CARTDIR"
__DIRECTORY_START__:

; Entry 0 - title sprite (mandatory)
off0=__LOADER_SIZE__+(__DIRECTORY_END__-__DIRECTORY_START__)
blocka=off0/1024
len0=__TITLE_SIZE__
	.byte	<blocka
	.word	off0 & $3ff
	.byte	$00
	.word	__TITLE_START__
	.word	len0

; Entry 1 - first executable (mandatory)
off1=off0+len0
block1=off1/1024
len1=__CODE_SIZE__+__RODATA_SIZE__+__DATA_SIZE__
	.byte	<block1
	.word	off1 & $3ff
	.byte	$88
	.word	__CODE_LOAD__
	.word	len1

; You may insert more entries in any format you like
; But I continue using the BLL-style entries. They have the length
; parameters xored with $ffff

.macro entry old_off, old_len, new_off, new_block, new_len, new_size, new_addr
new_off=old_off+old_len
new_block=new_off/1024
new_len=new_size
	.byte	<new_block
	.word	(new_off & $3ff) ^ $ffff
	.byte	$88
	.word	new_addr
	.word	new_len ^ $ffff
.endmacro

; The 2nd entry is the entertainment module that we run at startup
entry off1, len1, off2, block2, len2,__STARTUP_SIZE__+__INIT_SIZE__+__INTRO_CODE_SIZE__+__INTRO_RODATA_SIZE__+__INTRO_DATA_SIZE__, __STARTUP_LOAD__

; The 3rd entry is the 1st game level we want to run.
entry off2, len2, off3, block3, len3, __SKETCH_CODE_SIZE__+__SKETCH_RODATA_SIZE__+__SKETCH_DATA_SIZE__, __SKETCH_CODE_LOAD__

; The 4th entry should come here. It could be the 2nd level to run or data...

__DIRECTORY_END__:
