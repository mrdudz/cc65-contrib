.EXPORT _getspacestat

_getspacestat:
	lda $dc01
	and #16
  rts