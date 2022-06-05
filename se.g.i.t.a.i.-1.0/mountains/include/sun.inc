
.IFNDEF __SUN_INC
__SUN_INC:

.IMPORT __SCREEN1_LOAD__, __SCREEN2_LOAD__
.IMPORT vicbank

sunwinw=40
sunwinh=19

suncol=7
sunbgcol=6
sunbgcol2=15
sunbgcol3=14

sunemptychar = $fb

sunvicbank=vicbank
sunchr=$3800
sunscrn=__SCREEN1_LOAD__
sunscrnofs=sunscrn-sunvicbank
suncharofs=sunchr-sunvicbank
scrnofs2 = __SCREEN2_LOAD__ - sunvicbank

sund018val= < ( (sunscrnofs/$40)+(suncharofs/$0400) )
sund018val2= < ( (scrnofs2/$40)+(suncharofs/$0400) )

.ENDIF