;
; TETRIS for Commodore PET (using cc65 compiler)
; (c) Tim Howe, March 2010
;
; Core/accelerator functions
;

    .autoimport	on
    .case       on
    .debuginfo  off
    .importzp	sp, sreg, regsave, regbank, tmp1, ptr1, ptr2, ptr3
    ;.macpack	longbranch
    .export	_blit_board
    .export	_blit_tetromino_trans
    .export     _blit_tetromino_opaque
    .export     _wait_vsync
    .export     _rotate_left4
    .export     _rotate_right4
    .export     _rotate_left3
    .export     _rotate_right3
    .export     _get_time
    .export     _reset_time
    .export     _print_uint16_5
    .export     _print_uint8_2
    .export     _print_string
.ifdef DAC_TETRIS
    .export     _play_sample
.endif


; Defines that must match those in tetris.h
    .define BOARD_X         10
    .define BOARD_Y         20
    .define TETROMINO_SIZE  4

; Defines that must match those in platform.h
    .define SCREEN_WIDTH    40
    .define TIME            $8F     ; LS byte of jiffy clock


; ---------------------------------------------------------------
; void blit_board(unsigned char *board, unsigned char *screenpos)
; Copy the board array onto the screen at the offset provided
; Widths and heights are defined as constants above
; ---------------------------------------------------------------

.segment        "CODE"

.proc	_blit_board

    jsr     popax       ; Fetch ->screenpos
    sta     ptr2        ; Store lo
    stx     ptr2+1      ; Store hi

    jsr     popax       ; Fetch ->board
    sta     ptr1        ; Store lo
    stx     ptr1+1      ; Store hi

    ldx     #BOARD_Y    ; Line count

COPYLINE:
    ldy     #(BOARD_X-1)

BOARD1:
    lda     (ptr1),y
    sta     (ptr2),y
    dey
    bpl     BOARD1      ; Y = BOARD_X-1 downto 0

; Move down a line
    clc
    lda     ptr1
    adc     #BOARD_X
    sta     ptr1
    bcc     BOARD2
    inc     ptr1+1
BOARD2:
    clc
    lda     ptr2
    adc     #SCREEN_WIDTH
    sta     ptr2
    bcc     BOARD3
    inc     ptr2+1
BOARD3:

    dex
    bne     COPYLINE

    rts                 ; Params stack is already correct

.endproc




; ---------------------------------------------------------------
; void blit_tetromino_trans(unsigned char *cells, unsigned char *arraypos)
; Plots all the non-zero tetromino cells into the 'composite' board array
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_blit_tetromino_trans

    jsr     popax       ; Fetch ->arraypos
    sta     ptr2        ; Store lo
    stx     ptr2+1      ; Store hi

    jsr     popax       ; Fetch ->cells
    sta     ptr1        ; Store lo
    stx     ptr1+1      ; Store hi

    ldx     #TETROMINO_SIZE

TRANSLINE:
    ldy     #(TETROMINO_SIZE-1)

TRANS1:
    lda     (ptr1),y
    ;cmp     #$20
    beq     NEXT        ; Omit cells set to 0
    sta     (ptr2),y
NEXT:
    dey
    bpl     TRANS1      ; Y = TETROMINO_SIZE-1 downto 0

; Move down a line
    clc
    lda     ptr1
    adc     #TETROMINO_SIZE
    sta     ptr1
    bcc     TRANS2
    inc     ptr1+1
TRANS2:
    clc
    lda     ptr2
    adc     #BOARD_X
    sta     ptr2
    bcc     TRANS3
    inc     ptr2+1
TRANS3:

    dex
    bne     TRANSLINE

    rts                 ; Params stack is already correct

.endproc



; ---------------------------------------------------------------
; void blit_tetromino_opaque(unsigned char *cells, unsigned char *screenpos)
; Used to plot the 'next tetromino' directly onto the screen
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_blit_tetromino_opaque

    jsr     popax       ; Fetch ->screenpos
    sta     ptr2        ; Store lo
    stx     ptr2+1      ; Store hi

    jsr     popax       ; Fetch ->cells
    sta     ptr1        ; Store lo
    stx     ptr1+1      ; Store hi

    ldx     #TETROMINO_SIZE

OPAQUELINE:
    ldy     #(TETROMINO_SIZE-1)

OPAQUE1:
    lda     (ptr1),y
    bne     PLOT
    lda     #$20        ; Replace NULl cells with SPACE/BLACK
PLOT:
    sta     (ptr2),y
    dey
    bpl     OPAQUE1     ; Y = TETROMINO_SIZE-1 downto 0

; Move down a line
    clc
    lda     ptr1
    adc     #TETROMINO_SIZE
    sta     ptr1
    bcc     OPAQUE2
    inc     ptr1+1
OPAQUE2:
    clc
    lda     ptr2
    adc     #SCREEN_WIDTH
    sta     ptr2
    bcc     OPAQUE3
    inc     ptr2+1
OPAQUE3:

    dex
    bne     OPAQUELINE

    rts                 ; Params stack is already correct

.endproc


; ---------------------------------------------------------------
; void wait_vsync(void)
; Wait for an EVEN vsync interval (i.e. every-other vsync)
; This was changed from a simple wait for vsync to ensure that all
; pieces drop at the same rate when code optimisation is turned on.
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_wait_vsync

READTIME1:
    lda     TIME        ; Read 60Hz jiffy
    and     #1
    beq     READTIME1

    ; Now jiffy count is odd

    lda     TIME        ; Read 60Hz jiffy
READTIME2:
    cmp     TIME
    beq     READTIME2   ; Wait till it increments

    rts

.endproc



; ---------------------------------------------------------------
; Rotate tetrominos that occupy the whole 4x4 matrix
; void rotate_left4(unsigned char *src_cells, unsigned char *dest_cells)
; void rotate_right4(unsigned char *src_cells, unsigned char *dest_cells)

; Rotate tetrominos that occupy just the top left 3x3 matrix
; (This is required to stop them form appearing to rotate off-center)
; void rotate_left3(unsigned char *src_cells, unsigned char *dest_cells)
; void rotate_right3(unsigned char *src_cells, unsigned char *dest_cells)
; ---------------------------------------------------------------


.segment    "DATA"

ROT_LEFT4:
    .byte   00, 12, 01, 08, 02, 04, 03, 00
    .byte   04, 13, 05, 09, 06, 05, 07, 01
    .byte   08, 14, 09, 10, 10, 06, 11, 02
    .byte   12, 15, 13, 11, 14, 07, 15, 03

.segment	"CODE"

.proc	_rotate_left4

    jsr     popax       ; Fetch ->dest_cells
    sta     ptr2        ; Store lo
    stx     ptr2+1      ; Store hi

    jsr     popax       ; Fetch ->src_cells
    sta     ptr1        ; Store lo
    stx     ptr1+1      ; Store hi

    ldx     #00

ROT_L4:
    ldy     ROT_LEFT4, x
    lda     (ptr1),y
    inx
    ldy     ROT_LEFT4, x
    sta     (ptr2),y
    inx

    cpx     #32
    bne     ROT_L4

    rts

.endproc

.proc	_rotate_right4

    jsr     popax       ; Fetch ->dest_cells
    sta     ptr2        ; Store lo
    stx     ptr2+1      ; Store hi

    jsr     popax       ; Fetch ->src_cells
    sta     ptr1        ; Store lo
    stx     ptr1+1      ; Store hi

    ldx     #31

ROT_R4:
    ldy     ROT_LEFT4, x
    lda     (ptr1),y
    dex
    ldy     ROT_LEFT4, x
    sta     (ptr2),y
    dex

    cpx     #$FF
    bne     ROT_R4

    rts

.endproc


.segment    "DATA"

ROT_LEFT3:
    .byte   00, 08, 01, 04, 02, 00
    .byte   04, 09, 05, 05, 06, 01
    .byte   08, 10, 09, 06, 10, 02

.segment	"CODE"

.proc	_rotate_left3

    jsr     popax       ; Fetch ->dest_cells
    sta     ptr2        ; Store lo
    stx     ptr2+1      ; Store hi

    jsr     popax       ; Fetch ->src_cells
    sta     ptr1        ; Store lo
    stx     ptr1+1      ; Store hi

    ldx     #00

ROT_L3:
    ldy     ROT_LEFT3, x
    lda     (ptr1),y
    inx
    ldy     ROT_LEFT3, x
    sta     (ptr2),y
    inx

    cpx     #18
    bne     ROT_L3

    rts

.endproc

.proc	_rotate_right3

    jsr     popax       ; Fetch ->dest_cells
    sta     ptr2        ; Store lo
    stx     ptr2+1      ; Store hi

    jsr     popax       ; Fetch ->src_cells
    sta     ptr1        ; Store lo
    stx     ptr1+1      ; Store hi

    ldx     #17

ROT_R3:
    ldy     ROT_LEFT3, x
    lda     (ptr1),y
    dex
    ldy     ROT_LEFT3, x
    sta     (ptr2),y
    dex

    cpx     #$FF
    bne     ROT_R3

    rts

.endproc


; ---------------------------------------------------------------
; int get_time(void)
; Return 16 bits-worth of the 60Hz jiffy count
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_get_time

    sei                 ; Perform an atomic 2-byte read
    lda     TIME        ; Read LO byte of 60Hz jiffy count
    ldx     TIME-1      ; Read next byte of jiffy count
    cli
    rts

.endproc


; ---------------------------------------------------------------
; void reset_time(void)
; Clear the bottom 16 bits of the 60Hz jiffy count
; This will avoid a time-wrap condition for 18 minutes.
; This is a bit nasty and should be worked around better. TODO.
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_reset_time

    lda     #0
    sei                 ; Perform an atomic 2-byte write
    sta     TIME        ; Clear 60Hz jiffy
    sta     TIME-1      ; Clear jiffy*256
    cli
    rts

.endproc



; ---------------------------------------------------------------
; void print_uint16_5(UINT16 n)
; Convert a 16 bit binary value into a 5 digit bit BCD value
; then print it.
; (core from http://www.obelisk.demon.co.uk/6502/algorithms.html)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_print_uint16_5

    jsr     popax       ; Fetch n
    sta     ptr1+0      ; Store lo
    stx     ptr1+1      ; Store hi

    LDA     #0          ; Clear the result area
    STA     ptr2+0
    STA     ptr2+1
    STA     ptr2+2      ; I know Im using an extra byte here

    LDX     #16         ; Setup the bit counter
    SED                 ; Enter decimal mode

BCDLOOP5:
    ASL     ptr1+0      ; Shift MS bit out of the binary word
    ROL     ptr1+1      ; into the carry

    LDA     ptr2+0      ; Load the LS decimal digits
    ADC     ptr2+0      ; Add the carry in & double the value
    STA     ptr2+0
    LDA     ptr2+1      ; Load the next decimal digits
    ADC     ptr2+1      ; Add the carry in & double the value
    STA     ptr2+1
    LDA     ptr2+2      ; Load the next decimal digits
    ADC     ptr2+2      ; Add the carry in & double the value
    STA     ptr2+2
    DEX                 ; More bits to process?
    BNE     BCDLOOP5
    CLD                 ; Leave decimal mode

; Now print the result

    ldx     #5          ; # of decimal digits

    ;lda     ptr2+2     ; Ignore leading 0
    ;lsr
    ;lsr
    ;lsr
    ;lsr
    ;clc
    ;adc     #$30
    ;jsr     $FFD2     

    lda     ptr2+2      ; Digit 5
    and     #$0F
    clc
    adc     #$30
    jsr     $FFD2

    lda     ptr2+1      ; Digit 4
    lsr
    lsr
    lsr
    lsr
    clc
    adc     #$30
    jsr     $FFD2     

    lda     ptr2+1      ; Digit 3
    and     #$0F
    clc
    adc     #$30
    jsr     $FFD2

    lda     ptr2+0      ; Digit 2
    lsr
    lsr
    lsr
    lsr
    clc
    adc     #$30
    jsr     $FFD2     

    lda     ptr2+0      ; Digit 1
    and     #$0F
    clc
    adc     #$30
    jsr     $FFD2

    rts 

.endproc


; ---------------------------------------------------------------
; void print_uint8_2(UINT8 n)
; Convert an 8 bit binary value into a 2 digit BCD value
; then print it.
; (core from http://www.obelisk.demon.co.uk/6502/algorithms.html)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_print_uint8_2

    jsr     popa        ; Fetch n
    sta     ptr1+0      ; Store lo

    LDA     #0          ; Clear the result area
    STA     ptr2+0

    LDX     #8          ; Setup the bit counter
    SED                 ; Enter decimal mode

BCDLOOP2:
    ASL     ptr1+0      ; Shift MS bit out of the binary into the carry

    LDA     ptr2+0      ; Load the LS decimal digits
    ADC     ptr2+0      ; Add the carry in & double the value
    STA     ptr2+0
    DEX                 ; More bits to process?
    BNE     BCDLOOP2
    CLD                 ; Leave decimal mode

; Now print the result

    ldx     #2          ; # of decimal digits

PRTLOOP2:
    lda     ptr2+0      ; Digit 2
    lsr
    lsr
    lsr
    lsr
    clc
    adc     #$30
    jsr     $FFD2     

    lda     ptr2+0      ; Digit 1
    and     #$0F
    clc
    adc     #$30
    jsr     $FFD2

    rts 

.endproc


; ---------------------------------------------------------------
; void print_string(char *str)
; Print a NULL-terminated string (max length 255)
; Uses ROM COUT code for compactness.
; (At least ROM COUT is vectored for cross-PET compatibility.)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_print_string

    jsr     popax       ; Fetch -> string
    sta     ptr1+0      ; Store lo
    stx     ptr1+1      ; Store hi

    ldy     #0
STRINGLOOP:
    lda     (ptr1),y
    beq     ENDSTRING   ; Detect NULL termination
    jsr     $FFD2       ; ROM vector to COUT
    iny
    bne     STRINGLOOP
ENDSTRING:

    rts 

.endproc


.ifdef DAC_TETRIS
; ---------------------------------------------------------------
; void play_sample(unsigned char speed, unsigned char *sample, unsigned int length)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_play_sample

;   Get params from the params stack (and fix the stack as we go)

    jsr     popax       ; Fetch length
    sta     tmp1        ; Store lo
    stx     tmp1+1      ; Store hi

    jsr     popax       ; Fetch sample ptr
    sta     ptr1        ; Store lo
    stx     ptr1+1      ; Store hi

    jsr     popa        ; Fetch the delay param
    tax                 ; Store in X (be carful not to trash this!)

    lda     #$ff
    sta     $e843       ; Set user port to output

    ldy     #$00
    clc                 ; Used to achieve a 'branch always' below

    sei

SAMPLOOP:
    lda     (ptr1),y    ; Fetch sample
    sta     $e84f       ; Write to DAC

    inc     ptr1        ; Inc sample ptr
    bne     DECLENGTH
    inc     ptr1+1

DECLENGTH:
    lda     tmp1
    bne     DEC2
    dec     tmp1        ; Dec MS byte
    dec     tmp1+1      ; Dec LS byte
    bcc     DELAY       ; Branch always (needs carry to be clear)
DEC2:
    dec     tmp1        ; Only dec ls byte

DELAY:
    txa                 ; Recall delay value
    tay
DELAY2:
    dey
    bne     DELAY2      ; Delay

    lda     tmp1
    ora     tmp1+1      ; Test for length == 0
    bne     SAMPLOOP

    cli

    ;lda     #$00
    ;sta     $e843      ; Set user port back to input
                        ; Sounds better if this is not done till end of program

    rts                 ; The params stack is already back to normal


.endproc
.endif

; EOF


