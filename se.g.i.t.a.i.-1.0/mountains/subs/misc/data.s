.SEGMENT "CLOWDSPR"
.EXPORT _clowdspr
.EXPORT _clowdsprend,clowdsprdest
_clowdspr:
.INCBIN "datafiles/clowdsprbank1"
.INCBIN "datafiles/clowdsprbank2"
_clowdsprend:

.SEGMENT "CLOWDSPRBUF"
clowdsprdest:
.res $1c0


.SEGMENT "BERGCHAR"
.INCBIN "datafiles/bergcharset"



.SEGMENT "BERGSPR"
.EXPORT bergspr
bergspr:
	.INCBIN "datafiles/bergesprites"
		
		
.SEGMENT "SCREEN1"
.SEGMENT "SCREEN2"
.SEGMENT "CLOWDSPRBUF"
.SEGMENT "SPDCODE2"
;Just to declare compile time constants of start and length
;of the segments (__SEGMENTNAME_LOAD__ and __SEGMENTNAME_SIZE__)

