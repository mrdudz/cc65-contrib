.EXPORT setspry
.PROC setspry
					sta $d001
					sta $d003
					sta $d005
					sta $d007
					sta $d009
					sta $d00b
					sta $d00d
					sta $d00f
					rts
.ENDPROC