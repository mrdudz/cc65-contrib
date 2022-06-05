.IFNDEF __MOUNTAINS_INC
__MOUNTAINS_INC:
	.IMPORT __COLORRAM_START__, __SCREEN1_LOAD__
	.IMPORT vicbank, bergspr
	mountaincol1 = 7+8
	mountaincol2 = 10
	mountailcol3 = 2+8
	
	mountainy=20
	mountainx=$00
	mountainheight=6
	mountainxsize=6*6+3
	
	bergsprcol = 15
	bergsprcol2 = 11
	.IF DEBUG = 0
		bergsprcol3 = 9
	.ELSE
 		bergsprcol3 = 3
	.ENDIF
	bergsprx=3*8
	bergspry=mountainy*8+50+2
	bergsprxsize = 24
	bergsprxpand = 0
	bergsprypand = 0
	msbsprbit=0
	bergspr_mcolbit = $ff
	bergsprofs=bergspr-vicbank
	bergsprdef=bergsprofs/$40
	
	SCRW = 40
	bergscrladr=mountainy*$28+mountainx+__SCREEN1_LOAD__-$28
	bergcoladr=bergscrladr-__SCREEN1_LOAD__+__COLORRAM_START__
.ENDIF