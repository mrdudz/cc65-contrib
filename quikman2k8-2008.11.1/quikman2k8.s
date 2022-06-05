; Quikman 2K8 for Commodore VIC20
; written by Robert Hurst <robert@hurst-ri.us>
; using Commodore VICMON (SYS 45056)
; updated version: 01-Nov-2008
;
; to assemble this source using cc65.org project:
;   ca65.exe --cpu 6502 --listing quikman2k8.s
;   ld65.exe -C doc/vic20.cfg -o quikman2k8.prg quikman2k8.o
;
; to run the binary using viceteam.org project:
;   xvic -memory none -ntsc -sound -joydev1 2 -autostart quikman2k8.prg
;
; pertinent VIC20 symbols
JIFFYH		= $A0		; jiffy-clock hi byte value
JIFFYM		= $A1		; jiffy-clock mid byte value
JIFFYL		= $A2		; jiffy-clock low byte value
CLRPAGE		= $F4		; color memory page (unexpanded = $97)
SCRNPAGE	= $0288		; screen memory page (unexpanded = $1E)
CTRLCSHIFT	= $028D		; keyboard flag: control/commodore/shift
CASSBUFF	= $033C		; cassette buffer
VIC			= $9000		; start of video interface chip registers
RESET		= $FD22		; warm startup
CHROUT		= $FFD2		; print character with cursor translation
GETIN		= $FFE4		; get a character from keyboard queue
;
; my symbol / memory map
;			= $00		; base index (monster, sprite)
;			= $01		; index x2
;			= $02		; index x8
PPILLTIMER	= $10		; powerpill effectiveness timer
FRUITTIMER	= $39		; 0 - 242
FRUITFLAG	= $3A		; zero or non-zero, if fruit has been activated
PPILLFLAG	= $3B		; just ate a powerpill this turn (0=no)
CHOMP		= $3C		; pointer into sound effect for fruit and fleeing monsters
CHEWING		= $3D		; flag whether quikman just ate a dot or not
DIGIT		= $3E		; award points at this digit place
POINTS		= $3F		; how many points scored
OLDDIR		= $40		; direction sprite was last moving in
NEWDIR		= $41		; direction sprite wants to take, if valid by MAZEMOVE
JOYVAL		= $42		; last joystick read value
QMANDIR		= $43		; quikman's current direction (0=right,1=down,2=left,3=up)
LIVES		= $45		; 0 - 3
FLASHPILL	= $46		; powerpill blink counter (0-30)
EXTRAQMAN	= $47		; bonus quickman flag (0=unused)
DEMOQMAN	= $48		; spirit of quickman index (0-3)
FRUITLEVEL	= $49		; 0 - 12
DOTS		= $4A		; 0 - 174
PENALTY		= $4B		; $4B-$4E monsters are free-to-roam flag
;			= $4F		; $4F-$52 monsters current direction (0=right,1=down,2=left,3=up)
;			= $53		; $53,$55,$57,$59 monster's knowledge of quikman's "X" coord was
;			= $54		; $54,$56,$58,$5A monster's knowledge of quikman's "Y" coord was
;			= $61		; $61-$64 monster array for its next best move
;			= $69		; temporary var
FLEEINGSCORE= $70		; fleeing monster score: 2, 4, 8, 16
;			= $F7		; $F7/$F8,$F9,$FA are screen cell pointers for sprite's position
;			= $FE		; $FC/$FD,$FE/$FF are color cell pointers for same
;
; program indirects (my sprite registers)
SPRITE		= $02A1		; bitmask 0-7 controls sprite on/off
SPRITEX		= $02A2		; $02A2,A4,A6,A8,AA,AC,AE,B0 each "X" coordinate
SPRITEY		= $02A3		; $02A3,A5,A7,A9,AB,AD,AF,B1 each "Y" coordinate
SPRITECLR	= $02B2		; $02B2-$02B9 color
SPRITEIMG1	= $02BA		; $02BA-$02C1 low-byte of SPRITE image
SPRITEIMG2	= $02C2		; $02C2-$02C9 hi-byte of SPRITE image
SPRITELAST	= $02CC		; $02CC-$02DC keep last state of SPRITE registers
SAVEBACK	= $02DD		; $02DD-$02E6 keep what's under the sprite's 2-cell
;
; other constants
FRUITCELL	= $1F1B		; screen cell address of fruit
FRUITCELLCLR= $971B		; color cell address of fruit
;
; uses standard VIC20 (unexpanded)
;		.org $0FFF
;		.segment "STARTUP"
		.segment "LOADADDR"
;
;********************************************************************
; LOAD "QUIKMAN2K8.PRG",8,1
;		.word	$1001	; starting load address
		.segment "CODE"
		.org $1001
BASIC:	.byte	$0B, $10
				; 2008 SYS4109
		.byte	$D8, $07, $9E, $34, $31, $30, $39
		.byte	$00, $00, $00
;
;********************************************************************
; Main entry point into the game
START:
		LDA #$08		; lock uppercase / graphic set
		JSR CHROUT
		LDA VIC+$01
		SBC #$04
		STA VIC+$01
		LDA #$80+$15	; set for videoram @ $1E00 with 21-columns
		STA VIC+$02		; video matrix address + columns
		LDA #$B0		; $B0 = 10110000 = 24 rows + 8x8 height
		STA VIC+$03		; rows / character height
		LDX #$FF		; set for $1C00
		STX VIC+$05		; use programmable char set
		INX
		STX LIVES
		JSR RESTORE
		LDX #$15
@loop:	LDA BANNERMSG-1,X
		STA $1FE2,X
		LDA #$03
		STA $97E2,X
		DEX
		BNE @loop
		SEI
		LDX #<BACKGROUND
		LDY #>BACKGROUND
		STX $0314		; enable my IRQ vector jump
		STY $0315
		CLI
;
RESTART:
		JSR INITVARS
		LDY #$00		; mute
		STY VIC+$0E		; aux color / volume
		JSR GAMEOVER
		LDY #$0A
		STY SPRITE
@loop:	LDA JIFFYL
		PHA
		CMP #$02
		BCS @slow
		LDA VIC+$0F
		AND #$07
		TAX
		INX
		TXA
		ORA #$08
		STA VIC+$0F
		JSR MAZEPAINT
		LDA JIFFYM
		AND #$0F
		TAX
		CPX #$0D
		BCC @fruit
		LDX #$00
@fruit:	LDA FRUIT,X
		STA FRUITCELL
		LDA FRUITCLR,X
		STA FRUITCELLCLR
		LDA #$40
		STA FRUITTIMER
		STA FRUITFLAG
@slow:	PLA
		ADC #$01
@wait:	CMP JIFFYL
		BNE @wait
		LDA SPRITE
		EOR #$1E
		STA SPRITE
		JSR NPC			; demo mode
@scan:	JSR SPRITES
		JSR GETIN		; get keyboard
		CMP	#$03		; got STOP ?
		BNE @skip1
		JMP RESET		; bye-bye
@skip1:	CMP #$88		; got F7 ?
		BNE @loop		; try again ...
;
;********************************************************************
RESETGAME:
		JSR INITVARS
		LDA #$03		; start with 3-lives
		STA LIVES
		LDY #$FF		; -1 will become 0 at start of "next" level
		STY FRUITLEVEL
		JSR GAMEOVER	; clear status
		LDX #$00		; reset score
		STX EXTRAQMAN	; reset bonus
@loop1:	LDA #$B0		; each digit to "0"
		STA SCORE,X		; into savebuffer
		INX				; do next digit
		CPX #$06		; all 6 of them
		BNE @loop1
;
STARTLVL:
		JSR RESTORE		; initialize new level
;
RESETCHR:
		LDX #$00
		STX QMANDIR		; start off going RIGHT
		STX JOYVAL		; preload last joystick value as going RIGHT
@loop1:	LDA STARTPOS,X	; reset each sprite starting position
		STA SPRITEX,X
		INX
		CPX #$0A		; 5 sprites per X,Y coordinate pair
		BNE @loop1
		LDA #$E8
		STA SPRITEIMG1
		LDA #$1C		; always point quikman sprite image
		STA SPRITEIMG2	; to $1CE8 ... stupid?
		LDA #$1F		; turn on sprites 0-4
		STA SPRITE
		LDA #$05
		JSR PAUSE
		LDX #$05
@loop2:	LDA $C377,X		; print READY from ROM
		AND #$BF
		ORA #$80
		STA $1F18,X
		LDA #$07		; make it yellow
		STA $9718,X
		DEX
		BNE @loop2		; how geeky is that?
		LDA #$A0
		JSR PAUSE		; wait 2+ seconds
		LDX #$00
		LDA #$20		; erase READY
@loop4:	STA $1F19,X
		INX
		CPX #$05
		BNE @loop4
;
; zero $39 - $43
ZEROVARS:
		LDY #$39
		LDX #$00
@loop:	STX $00,Y
		INY
		CPY #$44
		BNE @loop
;
;********************************************************************
PLAYLOOP:
		LDA CTRLCSHIFT	; is the player holding down any
		BNE PLAYLOOP	; control, commodore, shift key(s)?
		LDA #$00
		LDX FRUITLEVEL
		CPX #$0C
		BCS @cont		; fastest!
		LDA #$06
		SEC
		SBC FRUITLEVEL	; progressive speed
		BEQ @cruz
		BCS @pace
@cruz:	LDA #$01
@pace:	JSR SLOWDOWN
@cont:	STA $00			; quikman is sprite #0
		STA $01
		LDA QMANDIR
		STA OLDDIR		; save last direction quikman was going in
		;++ lock-out changes to joystick
		SEI
		LDA JOYVAL
		STA NEWDIR		; do the same for the joystick
		JSR MAZEMOVE
		BCS @skip1		; is the direction valid?
		LDA JOYVAL		; yes,
		STA QMANDIR		; request quikman to move in direction of joystick
		CLC
		BCC @skip2
@skip1:	LDA QMANDIR
		STA NEWDIR
		JSR MAZEMOVE	; keep the current direction going?
@skip2:	LDA SPRITEX
		CLI
		;-- enable changes
		BNE @skip3		; is quikman at end of tunnel left?
		LDA #$9E
		STA SPRITEX		; put quikman at beginning of tunnel right
@skip3:	CMP #$A0		; is quikman at end of tunnel right?
		BNE @skip4
		LDA #$00
		STA SPRITEX		; put quikman at beggining of tunnel left
@skip4:	LDX #$00		; use X as a flag
		LDA SPRITEX
		AND #$07
		CMP #$04
		BNE @skip5		; is quikman in the middle of a left/right cell?
		INX
		LDA QMANDIR		; yes, 0=right, 2=left
		EOR #$02
		TAY
		BEQ @skip6		; going left, use 1st saveback cell
		DEY
		BNE @skip6		; going right, use overflow saveback cell
@skip5:	LDA SPRITEY
		AND #$07
		CMP #$04
		BNE @skip6		; is quikman in the middle of an up/down cell?
		INX
		LDY QMANDIR
		CPY #$01		; going down, use overflow saveback cell
		BEQ @skip6
		LDY #$00		; going up, use 1st saveback cell
@skip6:	CPX #$00
		BNE @skip7		; does quikman have something in its mouth?
		JMP NPCNEXT		; no, continue play
@skip7:	LDA SAVEBACK,Y	; retrieve the character from quikman's saveback buffer
		CMP #$20
		BNE @skip8		; nothing to eat, so move a bit faster
		JMP PLAYLOOP
		; check what was just eaten ...
@skip8:	TAX				; save that something in X
		LDA #$20
		STA SAVEBACK,Y	; replace the cell quikman is on with an empty space
		CPX #$1E		; is it a dot?
		BNE POWERUP
		LDA #$01
		STA POINTS		; score 1
		STA CHEWING		; quikman has to chew this dot, monsters keep movin'
		LDA #$0A		; score it @ 10-point digit
		STA DIGIT
;
POWERUP:
		CPX #$22
		BCC @skip1		; is X < 34 ?
		CPX #$2A		; no, is X >= 42 ?
		BCS NPCNEXT		; ate a piece of fruit?
		TXA				; YUMMY!
		SEC
		SBC #$22		; strip off char code for score index
		TAX
		LDA FRUITSCORE,X
		STA POINTS		; award points
		LDA #$09
		STA DIGIT		; in hundreds
		STA CHOMP
		CLC
		BCC NPCNEXT
@skip1:	CPX #$1F		; ate a powerpill?
		BNE EATING		; no, but I did eat a dot
		LDA #$05
		STA POINTS		; award 5-points
		LDA #$0A
		STA DIGIT		; score @ 10-digit
		STA PPILLFLAG
		BEQ NPCNEXT		; powerpills are dots on steroids, account for it
;
EATING:	BEQ WONLEVEL
		INC DOTS		; ate a dot, account for it
		LDA DOTS
		CMP #$AE		; are all dots eaten?
		BNE NPCNEXT
;===	achieved end of level	===
WONLEVEL:
		LDA #$80
		JSR PAUSE
		LDX #$FF
@loop:	JSR MAZEPAINT
		TXA
		AND #$1F
		BNE @next
		TXA
		PHA
		LDA SPRITE
		EOR #$1E
		STA SPRITE		; blink monsters
		JSR SPRITES
		PLA
		TAX
@next:	DEX
		CPX #$05
		BNE @loop
		LDA #$40
		JSR PAUSE
		JSR INITVARS
		LDX #$04
@loop1:	LDA PENALTY-1,X
		LDY FRUITLEVEL
		INY
@loop2:	LSR
		DEY
		BNE @loop2
		STA PENALTY-1,X		; after each level, the monsters dispatch quicker
		DEX
		BNE @loop1
		JMP STARTLVL
;
NPCNEXT:
		JSR GETIN		; get keyboard
		CMP	#$03		; got STOP ?
		BEQ WONLEVEL	; CHEATER!!
		JSR NPC
		JSR SPRITES
		JMP PLAYLOOP
;
;********************************************************************
; non-player characters & events
NPC:
		LDA FRUITFLAG
		BNE @skip3		; is fruit already on display?
		LDA DOTS
		CMP #$4B		; has the 75th dot been eaten?
		BEQ @skip1
		CMP #$7D		; has the 125th dot been eaten?
		BNE @skip3
@skip1:	LDX FRUITLEVEL	; prepare thy bonus
		CPX #$0C		; reach the last level?
		BCC @skip2
		LDX #$0C		; only the key is left, and it leaves a bad metallic after-taste
@skip2:	LDA FRUIT,X
		STA FRUITCELL	; display fruit
		LDA FRUITCLR,X
		STA FRUITCELLCLR
		LDA #$FA		; 250-moves and counting
		STA FRUITTIMER	; reset fruit timer
		STA FRUITFLAG
@skip3:	LDA FRUITTIMER	; fruit is on display
		BEQ @skip4		; nothing to do
		DEC FRUITTIMER	; remove a tick
		BNE @skip4		; there is still time left
		LDA #$20		; time's up!
		STA FRUITCELL	; no more fruit
@skip4:	LDA DOTS
		CMP #$4C		; has the 76th dot been eaten?
		BEQ @skip5
		CMP #$7E		; has the 126th dot been eaten?
		BNE @skip6
@skip5:	LDA #$00
		STA FRUITFLAG	; more fruit potential on this level
;
@skip6:	LDA PPILLFLAG
		BEQ @skip8		; just swallowed a powerpill?
		LDA FRUITLEVEL
		ASL				; multiply 8
		ASL
		ASL
		STA $FF			; shrink powerpill effectiveness per level
		LDA #$F8
		SEC
		SBC $FF
		STA PPILLTIMER	; set powerpill timer
		LDY #$04
@loop1:	LDX PENALTY-1,Y
		BNE @skip7		; is monster waiting in cage already?
		LDA #$06		; no, make monster blue
		STA SPRITECLR,Y
		LDA #$80		; make monster fleeing (0)
		STA SPRITEIMG1,Y
		LDA $4E,Y
		EOR #$02		; and reverse its direction
		STA $4E,Y
@skip7:	DEY
		BNE @loop1
@skip8:	LDA PPILLTIMER
		BEQ MOVING		; are monsters fleeing?
		CMP #$41		; yes ... but are they
		BCS @skipA		; getting confidence back?
		AND #$08		; yes, let's warn quikman
		BEQ @skipA
		LDY #$00
@loop2:	LDA SPRITEIMG1+1,Y
		CMP #$80		; is monster fleeing?
		BNE @skip9
		LDA SPRITECLR+1,Y
		EOR #$07		; flash white / blue
		STA SPRITECLR+1,Y
@skip9:	INY
		CPY #$04
		BNE @loop2
@skipA:	DEC PPILLTIMER	; drain powerpill
		BNE MOVING		; is there still power left?
;
; restore all monsters to their default colors and chase mode
CHASING:
		LDY #$04
@loop:	LDA MONSTERCLR-1,Y
		STA SPRITECLR,Y
		LDA #$78		; restore monster chasing image (/)
		STA SPRITEIMG1,Y
		DEY
		BNE @loop
;
MOVING:
		LDX #$00
		LDA PPILLFLAG
		BEQ KISSING		; just swallowed a powerpill?
		LDA #$00		; yes ...
		STA $3B			; account for that action
		LDA #$02		; start scoring @ 200-points
		STA FLEEINGSCORE
;
; check all monsters if any are in contact with quikman
KISSING:
		LDY #$00
KISSME:
		LDA LIVES
		BEQ NEXTKISS	; are we in demo mode?
		LDA SPRITEX
		CMP SPRITEX+2,Y
		BNE @skip3
		LDA SPRITEY
		SEC
		SBC SPRITEY+2,Y
		BCS @skip2
		EOR #$FF
@skip2:	CMP #$05
		BCS @skip3
		LDX #$FF
		BNE ENGAGED		; is quikman engaged with a monster?
@skip3:	LDA SPRITEY
		CMP SPRITEY+2,Y
		BNE NEXTKISS
		LDA SPRITEX
		SEC
		SBC SPRITEX+2,Y
		BCS @skip4
		EOR #$FF
@skip4:	CMP #$05
		BCS NEXTKISS
		LDX #$FF
		BNE ENGAGED		; is quikman engaged with a monster?
;
NEXTKISS:
		INY				; increment for next
		INY				; X,Y coord pair check
		CPY #$08
		BCC KISSME
		JMP MONSTERS	; quikman is still freely running!
;
ENGAGED:
		TYA
		LSR
		TAX
		LDA SPRITEIMG1+1,X
		CMP #$80		; is monster fleeing?
		BNE DEAD		; no, quikman bites the dust
		LDA #$09		; ahah!  caught a little sickly one!
		STA DIGIT		; in hundreds
		LDA FLEEINGSCORE
		STA POINTS		; fleeing monster score
		ASL				; next is worth x2 bonus
		STA FLEEINGSCORE
		LDA #$78
		STA SPRITEIMG1+1,X ; reset monster as chasing
		LDA MONSTERCLR,X
		STA SPRITECLR+1,X
		TXA
		ASL
		TAX
		LDA #$50		; reset "X" coord in cage
		STA $02A4,X
		LDA #$58		; reset "Y" coord in cage
		STA $02A5,X
		LDA #$09
		STA CHOMP
		LDA JIFFYL
@loop1:	CMP JIFFYL
		BEQ @loop1		; wait up to a jiffy
		TYA
		LSR
		TAX
		LDA CAGEDATA,X	; load waiting room time
		EOR #$FF
		LSR
		ADC #$20
		STA PENALTY,X	; monster is waiting
		JMP NEXTKISS	; is there another monster here?
;
DEAD:
		PLA				; remove quikman's call to NPC from stack
		PLA				; because he just died . . .
		LDA #$30
		JSR PAUSE
		; death sequence
		LDA #$01		; only feature quikman dying
		STA SPRITE
		LDA #$50		; low-order byte of 1st quikman image
		STA SPRITEIMG1
		LDA #$20
		STA $03A5		; rotate quikman 8 times
@loop1:	LDA #$1D		; 2nd page where quikman is on
		STA SPRITEIMG2
		LDA SPRITEIMG1
		CMP #$70		; are we at the 4th quikman image?
		BCC @skip
		LDA #$50		; reset to 1st quikman image
@skip:	CLC
		ADC #$08		; advance to next image
		STA SPRITEIMG1
		LDA #$04
		JSR PAUSE
		DEC $03A5
		BNE @loop1		; repeat next sequence
		LDA #$08
		STA SPRITEIMG1	; explode!
		LDA #$06
		JSR PAUSE
		JSR INITVARS
		DEC LIVES
		BEQ FINALITY	; any lives remaining?
		LDX LIVES
		LDA #$21
		STA $1FE2,X		; explode an avatar
		LDA #$06
		JSR PAUSE
		LDX LIVES
		LDA #$20
		STA $1FE2,X		; erase the avatar
		JMP RESETCHR	; quikman still has life -- try again!
;
FINALITY:
		LDY LIVES
		JSR GAMEOVER
		LDA #$00
		STA SPRITE
		LDA #$F0		; 4-second pause
		JSR PAUSE
		JMP RESTART		; this game is really over now
;
;********************************************************************
MONSTERS:
		SEI				; lock-out any background changes
		LDA #$01
		STA $00			; start with monster #1
;
DOMONSTER:
		ASL				; x2
		STA $01
		LDY $00
		LDX PENALTY-1,Y
		BEQ ITMOVES		; is this monster free to roam?
		DEX				; no, countdown to freedom
		STX PENALTY-1,Y
;
NEXTMONSTER:
		INC $00			; process next monster
		LDA $00
		CMP #$05		; all 5-sprites processed?
		BCC DOMONSTER
		LDA CHEWING
		CLI
		BNE @skip1		; is quikman eating a dot?
		RTS				; no, we're done
@skip1:	JSR SPRITES		; yes, chasing monsters get another turn
		JMP MONSTERS
;
ITMOVES:
		LDA PPILLTIMER
		AND #$03
		BEQ @cont		; powerpill active?
		LDA SPRITEIMG1,Y
		CMP #$80		; this monster IS fleeing
		BEQ NEXTMONSTER	; skip its turn
@cont:	LDX $01			; no, get pairing index
		LDA SPRITEX,X
		CMP #$50
		BNE @skip1
		LDA SPRITEY,X
		CMP #$58
		BNE @skip1		; is monster in cage ($50,$58 coord) doorway ?
		LDA #$57		; could have just used DEC SPRITEY,X instead
		STA SPRITEY,X	; move it a pixel UP to force it through the closed door
		LDX #$03
		STX $4E,Y		; make direction UP to get out of cage
		BNE @skip3
@skip1:	LDA SPRITEX,X
		BNE @skip2		; is monster against the left-side of the tunnel?
		LDX $00
		STA $4E,X		; force a change of direction to the right
@skip2:	CMP #$9F		; is monster against the right-side of the tunnel?
		BNE @skip3
		LDX $00
		LDA #$02
		STA $4E,X		; force a change of direction to the left
@skip3:	LDY #$00
		LDX #$00
@loop1:	STX $61,Y		; preset move priority as 0=right,1=down,2=left,3=up
		INY
		INX
		CPX #$04
		BCC @loop1
		LDY $01			; start of monster's calculated move
		LDA SPRITEX,Y
		AND #$07
		BEQ @skip4		; is monster horizontally aligned with a screen cell?
		LDA SPRITEY,Y
		AND #$07
		BNE @skip5		; is monster vertically aligned with a screen cell?
@skip4:	JSR AI			; yes, check to see if a direction change is in its future
		CLC
		BCC @skip6
@skip5:	LDX $4E,Y		; not in a position to make a direction change,
		STX $61			; so just keep monster going in its current direction
@skip6:	LDY #$00
		STY $04
@loop2:	LDX $61,Y
		TXA
		LDX $00
		EOR $4E,X
		CMP #$02
		BEQ @skip7		; don't allow monsters to reverse direction on their own
		LDX $61,Y
		STX NEWDIR
		LDY $00
		LDX $4E,Y
		STX OLDDIR
		JSR MAZEMOVE	; validate
		BCC MAKEMOVE	; is this a good move?
@skip7:	INC $04
		LDY $04
		CPY #$04
		BNE @loop2
		LDY $00			; reverse direction
		LDA OLDDIR
		EOR #$02
		STA $4E,Y
		JMP NEXTMONSTER
;
MAKEMOVE:
		LDY $00			; commit to this move
		LDX NEWDIR
		STX $4E,Y		; save as monster's current direction
		JMP NEXTMONSTER
;
; monster's artificial intelligence
AI:
; first, preload $61-$64 with "best" moves this monster can make
; to give quikman the kiss of death
		LDX $01
		LDA $51,X		; retrieve this monster's "X" knowledge where quikman was
		SEC
		SBC SPRITEX,X
		BCS @skip1
		LDY #$02
		STY $61			; LEFT is best
		LDY #$00
		STY $64			; RIGHT is worst
		BEQ @skip2
@skip1:	LDY #$00
		STY $61			; RIGHT is best
		LDY #$02
		STY $64			; LEFT is worst
@skip2:	LDA $52,X		; retrieve this monster's "Y" knowledge where quikman was
		SEC
		SBC SPRITEY,X
		BCS @skip3
		LDY #$03
		STY $62			; UP is 2nd best
		LDY #$01
		STY $63			; DOWN is 3rd best
		BNE AI2
@skip3:	LDY #$01		; DOWN is 2nd best
		STY $62
		LDY #$03		; UP is 3rd best
		STY $63
;
; next, prioritize monster move, based upon its current location in respect to
; its knowledge where quikman was considered last.
AI2:	LDX $01
		LDA $51,X
		SEC
		SBC SPRITEX,X
		BCS @skip1
		EOR #$FF
@skip1:	STA $69
		LDA $52,X
		SEC
		SBC SPRITEY,X
		BCS @skip2
		EOR #$FF
@skip2:	CMP $69
		BCC @skip3		; can monster improve upon order of choices?
		LDX $61			; swap 1st & 2nd choices
		LDY $62
		STX $62
		STY $61
		LDY $63			; swap 3rd & 4th choices
		LDX $64
		STY $64
		STX $63
@skip3:	LDY $00
		LDA SPRITEIMG1,Y
		CMP #$80		; is this monster fleeing?
		BNE @fini		; no, chase!
		LDX #$00
@loop1:	LDA $61,X
		PHA
		INX
		CPX #$04
		BNE @loop1
		LDX #$00
@loop2:	PLA
		STA $61,X		; reverse logic when in flee mode
		INX
		CPX #$04
		BNE @loop2
@fini:	RTS
;
INITVARS:
		LDX #$00
		STX SPRITE		; turn off all sprites
		LDY #$00
@loop:	LDX CAGEDATA,Y
		STX PENALTY,Y
		LDA STARTPOS,Y	; reset each sprite starting position
		STA SPRITEX,Y
		INY
		CPY #$10
		BNE @loop
		RTS
;
ADDSCORE:
		LDY POINTS
		BNE @skip1
		RTS
@skip1:	LDX DIGIT
@loop1:	LDA MAZEDATA,X
		CMP #$B9		; reach "9" ?
		BEQ @skip2
		INC MAZEDATA,X	; ding!
		DEY
		BNE @skip1
		RTS
@skip2:	LDA #$B0
		STA MAZEDATA,X	; wrap to "0"
		DEX				; and increment next order
		CLC
		BCC @loop1
;
JOYSTICK:
		LDX JOYVAL		; recall last joystick value
		LDA #$00
		STA $9113
		LDA #$7F
		STA $9122
		LDA $9120
		AND #$80		; JOY 3
		BNE @skip1
		LDX #$00
@skip1:	LDA #$FF
		STA $9122
		LDY $9111
		TYA
		AND #$08
		BNE @skip2
		LDX #$01
@skip2:	TYA
		AND #$10
		BNE @skip3
		LDX #$02
@skip3:	TYA
		AND #$04
		BNE @skip4
		LDX #$03
@skip4:	STX JOYVAL		; save
		RTS
;
; restore sound/screen
RESTORE:
		INC FRUITLEVEL
		LDA #$00
		STA SPRITE		; turn off all sprites
		TAX
@loop1:	STA VIC+$0A,X	; reset sound channels
		INX
		CPX #$04
		BNE @loop1
		LDA #$0E		; black / blue
		STA VIC+$0F		; background / border color
		LDA #$0F
		STA VIC+$0E		; volume
		LDA #$93		; Shift-HOME is clearscreen
		JSR	CHROUT		; print it
		LDX #$15		; skip 1st row -- scoring information
		LDY #$00
@draw:	LDA MAZEDATA,X
		STA $1E00,X
		LDA MAZEDATA+$0100,Y
		STA $1F00,Y
		INY
		INX
		BNE @draw
		STX DOTS		; and no dots are eaten (yet)
		LDX #$06		; blue
		JSR MAZEPAINT
		LDX #$05
@loop2:	LDA QUIKMANCLR-1,X ; reset monsters starting colors
		STA SPRITECLR-1,X ; into their sprite color registers
		DEX
		BNE @loop2
@loop3:	LDA #$78		; reset monsters as chasing
		STA SPRITEIMG1+1,X ; into their sprite register
		LDA #$1D		; reset monsters character
		STA SPRITEIMG2+1,X ; into their sprite register
		INX
		CPX #$04
		BNE @loop3
		LDX LIVES		; paint lives remaining
		DEX
		BEQ @next
@loop4:	LDA #$2D		; quikman character
		STA $1FE2,X		; bottom-left of screen
		LDA #$07		; use yellow
		STA $97E2,X		; and paint it
		DEX
		BNE @loop4
@next:	LDY FRUITLEVEL
@loop5:	CPY #$0C		; are we at the last level (key)?
		BCC @skip1
		LDY #$0C		; only keys remain
@skip1:	LDA FRUIT,Y		; fruit character
		STA $1FF1,X		; bottom right of screen
		LDA FRUITCLR,Y	; get its color
		STA $97F1,X		; and paint it
		CPY #$00		; did we paint the cherry yet?
		BEQ BEGIN		; if so, we're done
		INX
		STX $FF
		LDA FRUITLEVEL
		SEC
		SBC $FF
		TAY
		CPX #$07		; no more than 7 fruits to display
		BNE @loop5
BEGIN:	LDA LIVES		; allow new screen to be processed
		BEQ @cont
		LDA #$40
@cont:	JSR PAUSE		; by player
		RTS
;
; recolor maze with some new paint in X
MAZEPAINT:
		LDA #>MAZEDATA
		STA @loop+2
		LDA #$96
		STA @color+2
		LDY #$15
@loop:	LDA $1A00,Y
		CMP #$31
		BCC @skip1
		CMP #$3F
		BCS @skip1
		TXA
		AND #$07
@color:	STA $9600,Y
@skip1:	INY
		BNE @loop
		INC @loop+2
		INC @color+2
		LDA @color+2
		CMP #$98
		BNE @loop
		RTS
;
; animate the quikman
ANIMQMAN:
		LDX #$50		; closed mouth
		LDA JIFFYL
		AND #$04		; flip every 4-jiffies
		BEQ @skip1
		LDA QMANDIR		; take 0=right,1=down,2=left,3=up value
		ASL				; multiply by 8 to get address
		ASL
		ASL
		CLC
		ADC #$58		; add base offset
		TAX
@skip1:	LDY #$00
@loop1:	LDA $1D00,X		; copy a quikman image
		STA $1CE8,Y		; into its sprite
		INX
		INY
		CPY #$08
		BNE @loop1
		RTS
;
; if move is valid, carry flag will be clear on return
MAZEMOVE:
		LDY $01			; get X,Y coord index
		LDA OLDDIR		; get the last direction moving
		AND #$01		; mask UP/DOWN
		BEQ @skip1		; is direction LEFT/RIGHT?
		INY				; no, then fetch the "Y" coordinate
@skip1:	LDA SPRITEX,Y	; get one of sprite's coord
		AND #$07
		BEQ MAZEANY		; at a crossroad?  check move in any 4-directions
		LDA NEWDIR
		CMP OLDDIR
		BEQ MYMOVE		; still want to move in the same direction?
		EOR OLDDIR
		CMP #$02
		BEQ MYMOVE		; is this a reverse direction request?
		SEC				; no new move made
		RTS
;
MAZEANY:
		JSR SPRITEPREP
		LDA $F8			; reset screen hi-byte back into saved maze data
		SEC
		SBC #$04
		STA $F8
		LDX NEWDIR
		CPX #$02
		BCS @skip2		; is X (2=left) or (3=up)?
		LDA $F7			; no
		CLC
		ADC PEEKAHEAD,X	; look (0=right) or (1=down)
		BCC @skip1
		INC $F8
@skip1:	STA $F7
		CLC
		BCC @skip4		; go validate
@skip2:	LDA $F7
		SEC
		SBC PEEKAHEAD-2,X
		BCS @skip3		; look (2=left) or (3=up)
		DEC $F8
@skip3:	STA $F7
@skip4:	LDY #$00		; validate
		LDA ($F7),Y
		CMP #$31		; is this direction into a maze wall?
		BCC MYMOVE		; good move?
		RTS
;
; continue this sprite's move in whatever is loaded in NEWDIR
MYMOVE:
		LDA NEWDIR
		ASL				; 0=0, 1=2, 2=4, 3=6, 4=8
		TAX
		LDY $01
		LDA INERTIA,X
		CLC
		ADC SPRITEX,Y
		STA SPRITEX,Y
		LDA INERTIA+1,X
		CLC
		ADC SPRITEY,Y
		STA SPRITEY,Y
		CLC
		RTS
;
;********************************************************************
; my very own sprite routines
; major custom hack for this maze game implementation
SPRITES:
		LDA #$00		; start with sprite #0
		STA $00			; current sprite # to render
		;STA $02CB
@loop1:	ASL
		STA $01			; current sprite (x2) pairing index
		ASL
		ASL
		STA $02			; current sprite (x8) image index
		LDX $00
		LDA SPRITE
		AND SPRITEMASK,X
		BEQ @skip2		; nothing to do?
		LDA SPRITELAST	; what state was this sprite before?
		AND SPRITEMASK,X
		BEQ @skip1		; it was "off"
		JSR ERASESPRITE	; was "on" before, and we still want it "on"
@skip1:	JSR SPRITEPREP	; new sprite, go turn it "on"
		JSR PREPMATRIX
		JSR RENDER
		JSR PLACEMATRIX
		JMP @next
@skip2:	LDA SPRITELAST
		AND SPRITEMASK,X
		BEQ @next		; still nothing to do?  Then do nothing ...
		JSR ERASESPRITE	; make this sprite disappear
@next:	INC $00
		LDA $00
		CMP #$05		; only 5-sprites needed in this game
		BNE @loop1
		LDX #$00
@loop2:	LDA SPRITE,X	; save copy of current sprite registers
		STA SPRITELAST,X
		INX
		CPX #$11		; all 17 values, not including colors
		BNE @loop2
		RTS				; fini
;
; remove sprite from screen
ERASESPRITE:
		JSR LASTSPRITEPREP
		JSR PREPMATRIX
		JSR RESTOREMATRIX
		RTS
;
LASTSPRITEPREP:
		LDA $01			; 0, 2, 4, 6, 8
		CLC
		ADC #<SPRITELAST
		BNE SPRITEPREP2
;
; prepares the following registers:
; $F7/$F8,$F9/$FA		screen cell pointers for sprite position
; $FC/$FD,$FE/$FF		color cell pointers for same
SPRITEPREP:
		LDA $01			; 0, 2, 4, 6, 8 index
		CLC
		ADC #<SPRITE
SPRITEPREP2:
		TAX				; save this register index
		LDA $0201,X		; get "X" coordinate
		CMP #$A0
		BCC @skip1		; is "X" at or beyond last column?
		SBC #$A0		; yes, subtract 160-pixels wide
@skip1:	LSR				; and divide by 8-pixel width
		LSR
		LSR
		STA $F7			; save column offset from left screen
		STA $FC			; save column offset from left color
		LDA SCRNPAGE	; get high order byte of screen memory page
		STA $F8
		LDA CLRPAGE		; get high order byte of screen color page
		AND #$FE		; make it and "even" number
		STA $FD			; save high order
		LDA $0202,X		; get "Y" coordinate
		CMP #$B8
		BCC @skip2		; is "Y" at or beyond last row?
		SBC #$B8		; yes, subtract 184-pixels high
@skip2:	LSR				; and divide by 8-pixel height
		LSR
		LSR
		TAY
		BEQ @fini		; if on top row, no math required
		LDA $F7			; get column offset
@loop1:	CLC
		ADC #$15		; add 21 for next row
		BCC @skip3		; overflow to next page?
		INC $F8			; yes, increment high order bytes
		INC $FD
@skip3:	STA $F7			; save column offset
		STA $FC
		DEY
		BNE @loop1		; do for each "row"
@fini:	LDA $F8			; copy high-order bytes
		STA $FA			; for overflow sprite character
		LDA $FD			; do the same
		STA $FF			; for color
; determine whether overflow character is to the right or down
		LDA $0201,X		; get "X" coordinate
		AND #$07
		BEQ @vert		; 0 assumes moving up/down?
		LDA $F7			; ok, moving left/right then ...
		CLC
		ADC #$01		; make overflow character to the right
		BCC @savel
@saveh:	INC $FA
		INC $FF
@savel:	STA $F9			; save character offset
		STA $FE			; save color offset
		RTS
@vert:	LDA $F7
		CLC
		ADC #$15		; make overflow character below
		BCC @savel
		BCS @saveh
;
; prepares saveback buffers for restoring, should a larger-numbered sprite be
; overlapping any part of a smaller-numbered sprite
PREPMATRIX:
		LDY #$00
		LDA ($F7),Y		; retrieve screen cell
		PHA
		LDY $01			; 0, 2, 4, 6, 8 index
		LDA $0201,X
		AND #$07
		BNE @2cell
		LDA $0202,X
		AND #$07
		BEQ @start
@2cell:	LDY #$00
		LDA ($F9),Y		; retrieve overflow cell
		PHA
		LDY $01			; 0, 2, 4, 6, 8 index
		INY
@start:	TYA
		TAX
@loop:	PLA
@retry:	CMP $01
		BCC @skip		; is A < ME ?
		CMP #$0A
		BCS @skip		; is A >= MAX ?
; there is a sprite # greater than us on top ...
		TAY
		LDA SAVEBACK,Y	; get > sprite# saveback info
		CLC
		BCC @retry
@skip:	STA SAVEBACK,X
		DEX
		CPX $01
		BEQ @loop
		RTS
;
; restores the sprite's saveback buffer to the screen squares it occupies
; erasure part 3
RESTOREMATRIX:
		LDX $01
		LDA SAVEBACK,X	; recover character
		LDY #$00
		STA ($F7),Y		; restore to screen
		LDA #$01
		STA ($FC),Y		; just leave "white" behind
		LDA SPRITELAST+1,X
		AND #$07
		BNE @2cell
		LDA SPRITELAST+2,X
		AND #$07
		BEQ @fini
@2cell:	LDA SAVEBACK+1,X
		STA ($F9),Y		; restore to screen
		LDA #$01
		STA ($FE),Y		; only color overflow if X or Y are offset
@fini:	RTS
;
; render sprite within its character matrix by merging its image over its saveback
; $05/$06 points to graphic character
RENDER:
		LDX $00			; 0-4
		LDA SPRITEIMG1,X
		STA $05
		LDA SPRITEIMG2,X
		STA $06
		LDX $01			; 0,2,4,6,8
		LDA SPRITEY,X
		AND #$07
		STA $03
		TAX				; X will hold the sprite's Y coord
		LDY #$00		; erase temp image matrix area
		TYA
@loop1:	STA CASSBUFF+$20,Y
		INY
		CPY #$10		; customized from 4 to 2 character cells
		BNE @loop1
		TAY				; copy 8x8 character image into temp matrix
@loop2:	LDA ($05),Y		; $05/$06 points to character matrix to draw
		STA CASSBUFF+$20,X
		INX
		INY
		CPY #$08
		BNE @loop2
		LDX $01
		LDA SPRITEX,X
		AND #$07		; get modulos on X coordinate
		TAY
		BEQ @skip1		; if its zero, no shifting required
@loop3:	LDX #$00
@loop4:	CLC
		ROR CASSBUFF+$20,X
		ROR CASSBUFF+$28,X
		INX
		CPX #$08
		BNE @loop4
		DEY
		BNE @loop3
@skip1:	STY $FB			; Y is always zero here
@loop5:	LDA $01			; index x2
		CLC
		ADC $FB
		TAX				; X is sprite custom character
		LDA #$1C		; 1st page is where sprites are stored
		STA $06
		LDA SAVEBACK,X
		CMP #$80
		BCC @skip2		; is character reversed?
		LDY #$80		; yes, use start of ROM character set
		STY $06
@skip2:	AND #$1F		; get modulos of first 32-characters
		ASL				; and multiply by 8-pixel height
		ASL
		ASL
		STA $05			; save as low-order byte index
		LDA SAVEBACK,X
		AND #$60		; mask 01100000
		LSR				; divide by 16
		LSR
		LSR
		LSR
		LSR
		CLC
		ADC $06			; add result to high-order page index
		STA $06
		LDY #$00
		LDA $FB
		ASL
		ASL
		ASL
		TAX
@loop6:	LDA ($05),Y		; copy 8x8 character image into behind matrix
		STA CASSBUFF+$30,X
		INX
		INY
		CPY #$08
		BNE @loop6
		INC $FB
		LDA $FB
		CMP #$02		; only a 2-cell sprite now
		BNE @loop5
		LDY #$00
		LDA $02			; 0, 8, 16, 24, 32
		ASL				; 0, 16, 32, 48, 64
		TAX
@loop7:	LDA	CASSBUFF+$20,Y
		ORA CASSBUFF+$30,Y
		STA $1C00,X
		INX
		INY
		CPY #$10		; customized from 4 to 2 character cells
		BNE @loop7
		RTS
;
; puts the sprite character matrix on the screen
PLACEMATRIX:
		LDA $01			; 0, 2, 4, 6, 8
		LDY #$00
		STA ($F7),Y
		LDX $00
		LDA SPRITECLR,X
		STA ($FC),Y
		LDX $01
		LDA SPRITEX,X
		AND #$07
		BNE @color
		LDA SPRITEY,X
		AND #$07
		BEQ @fini
@color:	LDA ($FC),Y
		STA ($FE),Y		; only color overflow if X & Y are offset
		INX
		TXA
		STA ($F9),Y
@fini:	RTS
;
;********************************************************************
; This section is dedicated to background processing, accomplished
; via the keyboard IRQ service, called 60-times per second (jiffy).
BACKGROUND:
		LDA FLASHPILL	; powerpill flash
		CMP #$1E		; 30-jiffies?
		BNE @skip1
		LDX #$00		; reset counter
		STX FLASHPILL
@loop1:	LDA $1CF8,X		; custom graphic char
		EOR $8288,X		; rom graphic char
		STA $1CF8,X		; redraw 8x8 char cell
		INX
		CPX #$08
		BNE @loop1
		LDA #$FE		; render monster feet
		EOR $1D7F		; custom graphic char
		STA $1D7F		; redraw agressive monster
		STA $1D87		; redraw fleeing monster
@skip1:	INC FLASHPILL
		LDA LIVES
		BNE @hi			; playing?
		LDA JIFFYL		; manufacture a moving quikman 'spirit'
		AND #$03
		BEQ @hi
		CLC
		ADC JIFFYM
		AND #$03
		BEQ @hi
		INC DEMOQMAN
		LDA DEMOQMAN
		AND #$03
		ASL
		TAY				; for the monsters to 'chase'
		LDA CAGEDATA+$08,Y
		STA SPRITEX
		LDA CAGEDATA+$09,Y
		STA SPRITEY
@hi:	LDX #$00		; yes
@loop3:	LDA SCORE,X		; check current score against high score
		CMP MAZEDATA+$0F,X
		BCC @top		; is quikman beating the high score?
		BNE @skip4		; yes!
		INX
		CPX #$06
		BNE @loop3
@skip4:	LDX #$00
@loop4:	LDA SCORE,X		; woot!
		STA MAZEDATA+$0F,X
		INX
		CPX #$06
		BNE @loop4
@top:	LDX #$00		; refresh top line
@loop5:	LDA MAZEDATA,X
		STA $1E00,X
		INX
		CPX #$15
		BNE @loop5
		JSR ANIMQMAN	; update area pointed to by SPRITEIMG1&2
		JSR JOYSTICK
		JSR ADDSCORE
		STY POINTS
		LDX #$00
		LDY #$00
@loop6:	LDA PENALTY,X
		BNE @next		; not aware while caged
		LDA DOTS
		CMP #$AA		; make them all "smart" with 5-dots or less
		BCS @skip5
		LDA CAGEDATA,X
		BEQ @skip5		; is monster "smart"?  Red one is ...
		CMP JIFFYL		; no, so check as often as it waits
		BNE @next		; is its wait time equal to the jiffy clock?
@skip5:	LDA SPRITEX		; update this monster's awareness to where quikman is
		STA $53,Y
		LDA SPRITEY
		STA $54,Y
@next:	INY
		INY
		INX
		CPX #$04
		BNE @loop6
;
wahka:	LDA CHEWING
		BEQ @skip1
		LDA #$91		; start with an odd frequency
		STA VIC+$0C		; ignite a voice
@skip1:	LDA #$00		; dot is swallowed
		STA CHEWING
		LDA VIC+$0C
		BEQ @next1		; is this voice mute?
		LDA VIC+$0C
		AND #$01
		BEQ @skip3		; is it even?
		LDA VIC+$0C
		CLC
		ADC #$10		; increase tone
		CMP #$F1
		BCC @skip2		; is voice too high?
		SEC
		SBC #$01		; make it even
@skip2:	STA VIC+$0C
		CLC
		BCC @next1		; goto next effect
@skip3:	LDA VIC+$0C
		SEC
		SBC #$10		; drain tone
		STA VIC+$0C
@next1:	LDX CHOMP
		BEQ @skip4
		LDA SNDBIT,X	; load tone data
		STA VIC+$0B
		DEC CHOMP
@skip4:	LDA EXTRAQMAN
		BNE @fini		; already got bonus life
		LDA SCORE+1
		CMP #$B1		; did quikman just score 10,000-points?
		BNE @fini
		STA EXTRAQMAN
		LDX LIVES
		LDA #$2D
		STA $1FE2,X
		LDA #$07
		STA $97E2,X
		INC LIVES		; reward
@fini:	JMP $EABF		; jump to hardware IRQ
;
; Pass A for number of jiffies to wait
PAUSE:	STA @tick+1		; rewrite CMP operand below
		JSR SPRITES		; redraw sprites
		LDA JIFFYL
		STA $69			; save this jiffy
@loop:	LDA JIFFYL
		SEC
		SBC $69
@tick:	CMP #$00		; wait
		BNE @loop
		RTS
;
;********************************************************************
; Maze data ($1A00 - $1BFF)
; Screen size: 24-rows by 21-columns
		.res $1A00 - *
MAZEDATA:
SCORE	= * + $06
		.byte	$93, $83, $8F, $92, $85, $BA, $B0, $B0, $B0, $B0, $B0, $B0, $A0, $A0, $A0, $B0, $B2, $B0, $B0, $B0, $B0
		.byte	$37, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3D, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $38
		.byte	$39, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $39, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $39
		.byte	$39, $1E, $37, $38, $1E, $37, $3A, $3A, $38, $1E, $39, $1E, $37, $3A, $3A, $38, $1E, $37, $38, $1E, $39
		.byte	$39, $1F, $35, $36, $1E, $35, $3A, $3A, $36, $1E, $32, $1E, $35, $3A, $3A, $36, $1E, $35, $36, $1F, $39
		.byte	$39, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $39
		.byte	$39, $1E, $37, $38, $1E, $31, $1E, $33, $3A, $3A, $3D, $3A, $3A, $34, $1E, $31, $1E, $37, $38, $1E, $39
		.byte	$39, $1E, $39, $39, $1E, $39, $1E, $1E, $1E, $1E, $39, $1E, $1E, $1E, $1E, $39, $1E, $39, $39, $1E, $39
		.byte	$39, $1E, $35, $36, $1E, $3B, $3A, $3A, $34, $20, $32, $20, $33, $3A, $3A, $3C, $1E, $35, $36, $1E, $39
		.byte	$39, $1E, $1E, $1E, $1E, $39, $20, $20, $20, $20, $20, $20, $20, $20, $20, $39, $1E, $1E, $1E, $1E, $39
		.byte	$35, $3A, $3A, $38, $1E, $39, $20, $37, $3A, $3A, $C5, $3A, $3A, $38, $20, $39, $1E, $37, $3A, $3A, $36
		.byte	$3A, $3A, $3A, $36, $1E, $32, $20, $39, $20, $20, $20, $20, $20, $39, $20, $32, $1E, $35, $3A, $3A, $3A
		.byte	$20, $20, $20, $20, $1E, $20, $20, $35, $3A, $3A, $3A, $3A, $3A, $36, $20, $20, $1E, $20, $20, $20, $20
		.byte	$3A, $3A, $3A, $38, $1E, $31, $20, $20, $20, $20, $20, $20, $20, $20, $20, $31, $1E, $37, $3A, $3A, $3A
		.byte	$37, $3A, $3A, $36, $1E, $32, $20, $33, $3A, $3A, $3D, $3A, $3A, $34, $20, $32, $1E, $35, $3A, $3A, $38
		.byte	$39, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $39, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $39
		.byte	$39, $1E, $33, $38, $1E, $33, $3A, $3A, $34, $1E, $32, $1E, $33, $3A, $3A, $34, $1E, $37, $34, $1E, $39
		.byte	$39, $1F, $1E, $39, $1E, $1E, $1E, $1E, $1E, $1E, $20, $1E, $1E, $1E, $1E, $1E, $1E, $39, $1E, $1F, $39
		.byte	$3B, $34, $1E, $32, $1E, $31, $1E, $33, $3A, $3A, $3D, $3A, $3A, $34, $1E, $31, $1E, $32, $1E, $33, $3C
		.byte	$39, $1E, $1E, $1E, $1E, $39, $1E, $1E, $1E, $1E, $39, $1E, $1E, $1E, $1E, $39, $1E, $1E, $1E, $1E, $39
		.byte	$39, $1E, $33, $3A, $3A, $3E, $3A, $3A, $34, $1E, $32, $1E, $33, $3A, $3A, $3E, $3A, $3A, $34, $1E, $39
		.byte	$39, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E, $39
		.byte	$35, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $3A, $36
		.byte	$A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0, $A0
FRUITSCORE:
		.byte	$01, $03, $05, $07, $0A, $14, $1E, $32
;
;********************************************************************
; Custom character data -- must reside $1C00 - $1DFF
		.assert * = $1C00, error, "Graphics not at $1C00"
		.byte	$3E, $7C, $F8, $F0, $F0, $F8, $7C, $3E	; @ sprite #0 - quikman
		.byte	$00, $00, $00, $00, $00, $00, $00, $00	; A
		.byte	$00, $00, $00, $18, $18, $00, $00, $00	; B sprite #1 - red
		.byte	$00, $00, $00, $00, $00, $00, $00, $00	; C
		.byte	$38, $7C, $FE, $92, $FE, $FE, $FE, $AA	; D sprite #2 - green
		.byte	$00, $00, $00, $00, $00, $00, $00, $00	; E
		.byte	$00, $00, $00, $00, $00, $00, $00, $00	; F sprite #3 - cyan
		.byte	$00, $00, $00, $00, $00, $00, $00, $00	; G
		.byte	$38, $7C, $FE, $92, $FE, $FE, $FE, $AA	; H sprite #4 - yellow
		.byte	$00, $00, $00, $00, $00, $00, $00, $00	; I
;
;********************************************************************
; DATA
QUIKMANCLR:		; yellow
		.byte	$07
MONSTERCLR:		; red, green, cyan, yellow
		.byte	$02, $05, $03, $07
BANNERMSG:		; ©2008 RHURST  F7=PLAY
		.byte	$3F, $B2, $B0, $B0, $B8, $A0, $92, $88, $95, $92, $93, $94
		.byte	$A0, $A0, $86, $B7, $BD, $90, $8C, $81, $99
CAGEDATA:		;
		.byte	$00, $33, $76, $F9
		.byte	$02, $03, $02, $00
		.byte	$A0, $10, $00, $10, $A0, $B8, $00, $B8
FRUIT:			; cherry, strawberry, 2-peach, 2-apple, 2-pineapple, 2-tbird, 2-bell, key
		.byte	$22, $23, $24, $24, $25, $25, $26, $26, $27, $27, $28, $28, $29
FRUITCLR:		; red, red, 2-yellow, 2-red, 2-green, 2-magenta, 2-yellow, cyan
		.byte	$02, $02, $07, $07, $02, $02, $05, $05, $04, $04, $07, $07, $03
GOTEXT:			; GAME OVER
		.byte	$87, $81, $8D, $85, $A0, $8F, $96, $85, $92
INERTIA:		; maintain direction
		.byte	$01, $00, $00, $01, $FF, $00, $00, $FF
PEEKAHEAD:		;
		.byte	$01, $15
STARTPOS:		;
		.byte	$50, $88, $50, $48, $50, $58, $60, $58, $40, $58
SNDBIT:			; yummy sound effect
		.byte	$00, $00, $C0, $B8, $B0, $A8, $B0, $B8, $C0, $C8
SPRITEMASK:		; really?
		.byte	$01, $02, $04, $08, $10, $20, $40, $80
;
GAMEOVER:
		LDX #$09
@loop:	LDA GOTEXT-1,X	; GAME OVER
		CPY LIVES
		BEQ @dead
		LDA #$20		; space
@dead:	STA $1F16,X		; print character
		STA $1B16,X
		LDA #$02
		STA $9716,X
		DEX
		BNE @loop
		RTS
;
; A contains governor - all registers are reset to zero
SLOWDOWN:
		ASL
		TAY
		TAX
@sleep:	INX
		BNE @sleep
		DEY
		BNE @sleep
		TYA
		RTS
;
; resume graphic character data
		.res $1CE8 - *
		.byte	$3C, $7E, $FF, $FF, $FF, $FF, $7E, $3C	; ] pacman animated
		.byte	$00, $00, $00, $18, $18, $00, $00, $00	; ^ dot
		.byte	$00, $3C, $7E, $7E, $7E, $7E, $3C, $00	; <- powerpill animated
		.byte	$00, $00, $00, $00, $00, $00, $00, $00	;$20 empty space
		.byte	$92, $54, $28, $C6, $28, $54, $92, $00	; ! explosion
		.byte	$04, $08, $18, $24, $62, $F7, $F2, $60	; " cherry
		.byte	$10, $7C, $FE, $AA, $D6, $AA, $54, $28	; # strawberry
		.byte	$20, $10, $7C, $FE, $FE, $FE, $7C, $38	; $ peach
		.byte	$08, $10, $7C, $FE, $FE, $FE, $7C, $28	; % apple
		.byte	$08, $10, $38, $38, $7C, $FE, $FE, $6C	; & pear
		.byte	$10, $30, $92, $FE, $7C, $38, $10, $28	; ' tbird
		.byte	$10, $38, $7C, $7C, $7C, $7C, $FE, $10	; ( bell
		.byte	$18, $24, $18, $08, $08, $18, $08, $18	; ) key
		.byte	$3C, $7E, $FF, $FF, $FF, $FF, $7E, $3C	; * pacman closed
		.byte	$3E, $7C, $F8, $F0, $F0, $F8, $7C, $3E	; + pacman right
		.byte	$3C, $7E, $FF, $FF, $E7, $C3, $81, $00	; , pacman down
		.byte	$7C, $3E, $1F, $0F, $0F, $1F, $3E, $7C	; - pacman left
		.byte	$00, $81, $C3, $E7, $FF, $FF, $7E, $3C	; . pacman up
		.byte	$38, $7C, $FE, $92, $FE, $FE, $FE, $AA	; / ghost chasing
		.byte	$38, $7C, $FE, $92, $FE, $82, $FE, $54	; 0 ghost fleeing
		.byte	$18, $24, $42, $42, $42, $42, $42, $42	; 1 maze wall north
		.byte	$42, $42, $42, $42, $42, $42, $24, $18	; 2 maze wall south
		.byte	$00, $3F, $40, $80, $80, $40, $3F, $00	; 3 maze wall west
		.byte	$00, $FC, $02, $01, $01, $02, $FC, $00	; 4 maze wall east
		.byte	$42, $41, $40, $40, $40, $20, $1F, $00	; 5 maze wall s-w elbow
		.byte	$42, $82, $02, $02, $02, $04, $F8, $00	; 6 maze wall s-e elbow
		.byte	$00, $1F, $20, $40, $40, $40, $41, $42	; 7 maze wall n-w elbow
		.byte	$00, $F8, $04, $02, $02, $02, $82, $42	; 8 maze wall n-e elbow
		.byte	$42, $42, $42, $42, $42, $42, $42, $42	; 9 maze wall vertical
		.byte	$00, $FF, $00, $00, $00, $00, $FF, $00	; : maze wall horizontal
		.byte	$42, $41, $40, $40, $40, $40, $41, $42	; ; maze wall west tee
		.byte	$42, $82, $02, $02, $02, $02, $82, $42	; < maze wall east tee
		.byte	$00, $FF, $00, $00, $00, $00, $81, $42	; = maze wall north tee
		.byte	$42, $81, $00, $00, $00, $00, $FF, $00	; > maze wall south tee
		.byte	$3C, $42, $99, $A1, $A1, $99, $42, $3C	;(C)copyright symbol
