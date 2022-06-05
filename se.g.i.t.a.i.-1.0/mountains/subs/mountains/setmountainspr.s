.EXPORT setmountainspr

.INCLUDE "misc_constants.inc"
.INCLUDE "mountains.inc"

.PROC setmountainspr
	.IMPORT msb, mountainxcor, defs, setspry,scrllmountains
	.IMPORT _bergsprcol, _bergsprcol2, _bergsprcol3
          lda #bergspry
          jsr setspry

			    lda defs+0
					sta sprdefptr1+0
				
					 lda defs+1
					sta sprdefptr1+1
					
					 lda defs+2
					sta sprdefptr1+2
					
					 lda defs+3
					sta sprdefptr1+3
					
					 lda defs+4
					sta sprdefptr1+4
					
					 lda defs+5
					sta sprdefptr1+5
					
					 lda defs+6
					sta sprdefptr1+6
					
					 lda defs+7
					sta sprdefptr1+7
					                  
					lda #$1d
          sta $d011
          
					lda #bergspry-2
					cmp $d012
					bne *-3
					 
.IF ::DEBUG
					inc $d020
.ELSE
					lda (0,x)
.ENDIF
					lda #$ff
					sta $d01b
					sta $d015
					lda #$00
					sta $d01d
					                           
          lda _bergsprcol3
          sta $d026
          lda _bergsprcol2
          sta $d025
          
          lda _bergsprcol
          sta $d027
          sta $d028
          sta $d029
          sta $d02a
          sta $d02b
          sta $d02c
          sta $d02d
          sta $d02e
										  					
          lda #vicadr
					sta $d018	
					
					lda msb
          sta $d010
					lda mountainxcor
          sta $d000
          lda mountainxcor+1
          sta $d002
          lda mountainxcor+2
          sta $d004
          lda mountainxcor+3
          sta $d006
          lda mountainxcor+4
          sta $d008
          lda mountainxcor+5
          sta $d00a
          lda mountainxcor+6
          sta $d00c
          lda mountainxcor+7
          sta $d00e
           
          rts
.ENDPROC