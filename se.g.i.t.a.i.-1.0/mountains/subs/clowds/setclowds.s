.FEATURE labels_without_colons

.INCLUDE "misc_constants.inc"
.INCLUDE "clowds.inc"

.EXPORT setclowds
.PROC setclowds
	.IMPORT clowdmsb, clowddef, clowdxkor
	.IMPORT setspry
	.IMPORT _clowdsprcolor, _clowdsprcolor2, _clowdsprcolor3
					lda #clowdspry
          jsr setspry
          
          lda #0
          sta $d01b
          lda #$ff
          sta $d01d
          
          lda clowdxkor+0;#<0*CLOWDSPRXSIZE + CLOWDSPRX 
          sta $d000
          lda clowdxkor+1;#<1*CLOWDSPRXSIZE + CLOWDSPRX 
          sta $d002
          lda clowdxkor+2;#<2*CLOWDSPRXSIZE + CLOWDSPRX 
          sta $d004
          lda clowdxkor+3;#<3*CLOWDSPRXSIZE + CLOWDSPRX 
          sta $d006
          lda clowdxkor+4;#<4*CLOWDSPRXSIZE + CLOWDSPRX 
          sta $d008
          lda clowdxkor+5;#<(5*CLOWDSPRXSIZE + CLOWDSPRX )
          sta $d00a
          lda clowdxkor+6;#<(6*CLOWDSPRXSIZE + CLOWDSPRX) 
          sta $d00c
          lda clowdxkor+7;#<(6*CLOWDSPRXSIZE + CLOWDSPRX) 
          sta $d00e
          
          .IF ::DEBUG = 0 
	          lda _clowdsprcolor
	          sta $d027
	          sta $d028
	          sta $d029
	          sta $d02a
	          sta $d02b
	          sta $d02c
	          sta $d02d
	          sta $d02e
          .ELSE
          	ldx #1
          	stx $d027
          	inx 
          	stx $d028
          	inx
          	stx $d029
          	inx
          	stx $d02a
          	inx
          	stx $d02b
          	inx
          	inx ;avoid sun bkgcolor
          	stx $d02c
          	inx
          	stx $d02d
          	inx
          	stx $d02e
          .ENDIF
          
          lda _clowdsprcolor2
          sta $d025
          lda _clowdsprcolor3
          sta $d026
                          	
					lda clowddef+0
          sta sprdefptr1+0
          sta sprdefptr2+0
          lda clowddef+1
          sta sprdefptr1+1
          sta sprdefptr2+1
          lda clowddef+2
          sta sprdefptr1+2
          sta sprdefptr2+2
          lda clowddef+3
          sta sprdefptr1+3
          sta sprdefptr2+3
          lda clowddef+4
          sta sprdefptr1+4
          sta sprdefptr2+4
          lda clowddef+5
          sta sprdefptr1+5
          sta sprdefptr2+5
          lda clowddef+6
          sta sprdefptr1+6
          sta sprdefptr2+6
          lda clowddef+7
          sta sprdefptr1+7
          sta sprdefptr2+7
          
          lda clowdmsb
          sta $d010
          
          lda #$ff
          sta $d015
          rts
.ENDPROC