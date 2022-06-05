; Handy Atari Lynx cart header.
;
; Written for the cc65 compiler by
; Karri Kaksonen, 2004
;

LynxBlockSize=1024	; This should be a system-level parameter
			; when we have encrypted loaders for 512
			; and 2048 byte blocks also

.segment	"CARTDIR"
	; Here is a small header for Handy - the Lynx emulator
	; It is included to make testing easier
	.byte	'L','Y','N','X'			  ; magic
	.word	LynxBlockSize			  ; bank 0 page size
	.word	LynxBlockSize			  ; bank 1 page size
	.word	1				  ; version number
	.asciiz	"My really, really cool game!!!!" ; 32 bytes cart name
	.asciiz "Great Classics "		  ; 16 bytes manufacturer name
 	.byte	0				  ; rotation 1=left, 2=right
	.byte	0,0,0,0,0			  ; spare

