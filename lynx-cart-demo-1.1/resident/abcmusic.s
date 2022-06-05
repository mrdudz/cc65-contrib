; The abcmusic for the Lynx is based on the abcmusic notation
; but the implementation is made by Karri Kaksonen
; The idea is that you call update_music at regular intervals
; to keep the music going. I usually use a VBL interrupt to tick
; the metronome and call the update_music routine every time the
; metronome has changed.

; The Lynx hardware keeps the sound going on without CPU support.
; The update_music will just control the fade of notes, change of
; notes as the mosic goes on etc.

; In addition to call update_music frequently you need to define
; what to play. Calling "silence" is a good thing to do at startup.
; Otherwise some random noise may occur.

; The last external entry point "abcmusic" needs two parameters:
; channel (0..3) and an ASCII string with the tune to play on this
; channel.

; Command reference
; | Start of section
; :| Repeat section
; On, n = 0..6 Octave
; Xn, n = 0..511 XOR taps setting
; Yn, n = 0..4095 backup setting for XOR taps
; In, n = 0..3 Integrate bit 1 waveform bit 2 sweep
; Tn, n = 0..255 Tempo
; Vn, n = 0..127 Volume
; Rn, n = 0..127 Ramp up (Attack)
; Hn, n = 0..255 Hold
; Kn, n = 0..127 Kill sound (Decay)
; =, up half tone
; ~, down half tone
; CDEFGABcdefgab, notes
; z, pause
; n[=~][CDEFGABcdefgabz], n = 2,3,4

; ABC music definition file
	.include "lynx.inc"
	.include "../resident/extzp.inc"
	.export _silence
	.export _abc_update
	.export _abcmusic
	.export _abcinstrument
	.export _abc_music_ptr
	.import popax, popa
	.import _metronome

; vars for ABC-music

	.rodata
abcsilence:  .byte 0

	.code
; Set all scores to silent at startup
_silence:
  lda #$00
  sta $fd50 ; all channels to left+right
  lda #<abcsilence
  ldy #>abcsilence
  ldx #0
  jsr _abc_set_score
  lda #<abcsilence
  ldy #>abcsilence
  ldx #1
  jsr _abc_set_score
  lda #<abcsilence
  ldy #>abcsilence
  ldx #2
  jsr _abc_set_score
  lda #<abcsilence
  ldy #>abcsilence
  ldx #3
  jsr _abc_set_score
  lda #<abcsilence
  ldy #>abcsilence
  ldx #4
  jmp _abc_set_score

; Read a unsigned interger
; A contains low part of nr.
; X is channel, Y is music ptr
abc_read_number:
  stz abc_tmp
  stz abc_tmp+1
@L1:
  jsr abc_read_char ; Y may be incremented
  cmp #$2f
  bcc @L2
  cmp #$39
  bcs @L3
  bra @L5
@L2:
  cmp #0
  beq @L4
@L3:
  dey		; It was not 0, decrement music ptr
@L4:
  lda abc_tmp
  rts
@L5:
  and #$0f
  phx		; Save X
  pha
  asl abc_tmp	; multiply by 2
  rol abc_tmp+1
  lda abc_tmp
  ldx abc_tmp+1
  asl abc_tmp	; multiply by 4
  rol abc_tmp+1
  asl abc_tmp
  rol abc_tmp+1 ; multiply by 8
  clc		; add (multiply by 2) = 10
  adc abc_tmp
  sta abc_tmp
  txa
  adc abc_tmp+1
  sta abc_tmp+1
  pla		; add read character
  clc
  adc abc_tmp
  sta abc_tmp
  plx		; Restore X
  lda #0
  adc abc_tmp+1
  sta abc_tmp+1
  bra @L1

_abcmusic:
  lda _metronome
  ldx _metronome+1
  cmp _metronome
  bne _abcmusic
  cpx _metronome+1
  bne _abcmusic
  sta abcmetronome
  stx abcmetronome+1
  jsr popax   ; pop string
  sta abc_tmp
  stx abc_tmp+1
  jsr popa   ; pop channel
  tax
  lda abc_tmp
  ldy abc_tmp+1
; Activate score on channel X
; A - low address
; Y - high address
_abc_set_score:
  inx
  dex
  bne abc_set_score2
    sta _abc_score_ptr0
    sty _abc_score_ptr0+1
    sty abc_repeat_hoffs
    stz _abc_music_ptr
    stz sound_channel_duration
    rts
abc_set_score2:
  dex
  bne abc_set_score4
    sta _abc_score_ptr1
    sty _abc_score_ptr1+1
    sty abc_repeat_hoffs+1
    stz _abc_music_ptr+1
    stz sound_channel_duration+1
    rts
abc_set_score4:
  dex
  bne abc_set_score6
    sta _abc_score_ptr2
    sty _abc_score_ptr2+1
    sty abc_repeat_hoffs+2
    stz _abc_music_ptr+2
    stz sound_channel_duration+2
    rts
abc_set_score6:
  sta _abc_score_ptr3
  sty _abc_score_ptr3+1
  sty abc_repeat_hoffs+3
  stz _abc_music_ptr+3
  stz sound_channel_duration+3
  rts

; Once at each frame we can update the music
; You should call this routine frequently.
; Once in a frame is a good idea.
_abc_update:
  lda _metronome
  ldx _metronome+1
  cmp _metronome
  bne _abc_update
  cpx _metronome+1
  bne _abc_update
  cmp abcmetronome
  bne @L1
  cpx abcmetronome+1
  bne @L1
  rts
@L1:
  sta abcmetronome
  stx abcmetronome+1
  ldx #0
update_channel_x:

  ; Decrement steady snd duration, set max_vol to 0 if snd ended
  lda sound_channel_maxlen,x
  beq update_music1
    dea
    sta sound_channel_maxlen,x
    bne update_decay
update_music1:
      sta sound_channel_max_volume,x

  ; Decay sound
update_decay:
  lda sound_channel_max_volume,x
  bne update_attack
    ; silence
    lda sound_channel_volume,x
    beq update_duration
      ; decay time still going on
      sec
      sbc abc_instrument_decr,x
      bcs update_set_vol
        ; silence
        lda #0
      bra update_set_vol

  ; Attack sound
update_attack:
  lda sound_channel_volume,x
  cmp sound_channel_max_volume,x
  beq update_duration
    ; attack time
    clc
    adc abc_instrument_incr,x
    bcs update_music3
    cmp sound_channel_max_volume,x
    bcc update_set_vol
update_music3:
      ; desired volume reached
      lda sound_channel_max_volume,x

  ; Set volume
update_set_vol:
  sta sound_channel_volume,x
  phx
  pha
  txa
  clc
  rol
  clc
  rol
  clc
  rol
  tax
  pla
  sta $fd20,x
  plx

  ; Decrement duration of note
update_duration:
  lda sound_channel_duration,x
  beq update_music4
    dea
    sta sound_channel_duration,x
    bra update_music5
update_music4:
    ; note has ended, fetch next
    lda _abc_music_ptr,x
    tay
    jsr parse_abc

update_music5:
  inx
  txa
  cmp #4
  bne update_channel_x
  rts

; Parse score enough to get next note
; X - channel to use
; Y - abc music pointer
parse_abc:
  jsr abc_read_char
  cmp #$0 ; End of music
  bne parse_abc1
    rts
parse_abc1:
  cmp #$20 ;' ' ignore spaces
  bne parse_abc2
    bra parse_abc
parse_abc2:
  cmp #$7c ;'|'
  bne parse_abc6
    jsr abc_read_char
    cmp #$3a ;':'
    bne parse_abc4
      tya
      sta abc_repeat_offs,x
      phx
      txa
      asl
      tax
      lda _abc_score_ptr0+1,x
      plx
      sta abc_repeat_hoffs,x
      lda #2
      sta abc_repeat_cnt,x
    bra parse_abc5
parse_abc4:
      dey
parse_abc5:
    jmp parse_abc
parse_abc6:
  cmp #$3a ;':'
  bne parse_abc7
    lda abc_repeat_cnt,x
    dea
    sta abc_repeat_cnt,x
    beq parse_abc7
      phy
      lda abc_repeat_hoffs,x
      pha
      txa
      asl
      tax
      pla
      sta _abc_score_ptr0+1,x
      txa
      clc
      ror
      tax
      lda abc_repeat_offs,x
      tay
      pla
      sta abc_repeat_offs,x
      jmp parse_abc
parse_abc7:
  cmp #'V' ; volume
  bne parse_abc8
    jsr abc_read_number
    sta abc_note_volume,x
    jmp parse_abc
parse_abc8:
  cmp #'R' ; ramp up
  bne parse_abc8a
    jsr abc_read_number
    sta abc_instrument_incr,x
    jmp parse_abc
parse_abc8a:
  cmp #'H' ; hold
  bne parse_abc8b
    jsr abc_read_number
    sta abc_instrument_maxlen,x
    jmp parse_abc
parse_abc8b:
  cmp #'K' ; kill sound
  bne parse_abc8c
    jsr abc_read_number
    sta abc_instrument_decr,x
    jmp parse_abc
parse_abc8c:
  cmp #'I' ; incremental flag
  bne parse_abc9
    jsr abc_read_number
    phx
    txa
    clc
    rol
    clc
    rol
    clc
    rol
    tax
    lda abc_tmp
    cmp #0
    bne parse_abc8d
      lda $fd25,x
      and #$df
    bra parse_abc8e
parse_abc8d:
      lda $fd25,x
      ora #$20
parse_abc8e:
    ora #$18
    sta $fd25,x
    plx
    jmp parse_abc
parse_abc9:
  cmp #$54 ;'T' tempo
  bne parse_abc9b
    jsr abc_read_number
    sta abc_note_length,x
    jmp parse_abc
parse_abc9b:
  cmp #$4f ;'O' octave
  bne parse_abc9c
    jsr abc_read_number
    phx
    txa
    clc
    rol
    clc
    rol
    clc
    rol
    tax
    lda $fd25,x
    and #$f8
    ora #$18
    ora abc_tmp
    sta $fd25,x
    plx
    jmp parse_abc
parse_abc9c:
  cmp #'X'; XOR taps
  bne parse_abc10
    jsr abc_read_number
    phx
    txa ; modify X to point to sound channel
    clc
    rol
    clc
    rol
    clc
    rol
    tax
    lda abc_tmp
    ; The two topmost bits are shifted one place right
    bpl parse_abc9d
      ora #$40
    bra parse_abc9e
parse_abc9d:
      and #$bf
parse_abc9e:
    dec abc_tmp+1
    beq parse_abc9f
      and #$7f
    bra parse_abc9g
parse_abc9f:
      ora #$80
parse_abc9g:
    sta $fd21,x
    ; Bit $40 is put in a different register
    lda abc_tmp
    and #$40
    bne parse_abc9h
      lda $fd25,x
      and #$7f
    bra parse_abc9i
parse_abc9h:
      lda $fd25,x
      ora #$80
parse_abc9i:
    ora #$18
    sta $fd25,x
    lda #0
    sta $fd23,x
    plx
    jmp parse_abc
parse_abc10:
  cmp #'Y'; XOR taps
  bne parse_abc10a
    jsr abc_read_number
    phx
    txa ; modify X to point to sound channel
    clc
    rol
    clc
    rol
    clc
    rol
    tax
  lda $fd25,x
  and #$f7
  sta $fd25,x
    lda abc_tmp
    sta $fd23,x
    lda abc_tmp+1
    asl
    asl
    asl
    asl
    sta $fd27,x
  lda $fd25,x
  ora #$08
  sta $fd25,x
    plx
    jmp parse_abc
parse_abc10a:
  cmp #$7a ;'z'
  bne parse_abc11
    lda #0
    jmp set_music_ptr
parse_abc11:
  ; Find out the pitch of the note
  stz cur_note
  inc cur_note
  cmp #'^';
  bne parse_abc12
    inc cur_note
    jsr abc_read_char
parse_abc12:
  cmp #'_';
  bne parse_abc13
    dec cur_note
    jsr abc_read_char
parse_abc13:
  sec
  sbc #$41 ;'A'
  cmp #8 ;'H'-'A'
  bcs parse_abc14 ;_IFLO
    clc
    asl
    clc
    adc cur_note
    sta cur_note
  bra parse_abc15
parse_abc14:
    sec
    sbc #$20 ;'a'-'A' + 15
    clc
    asl
    clc
    adc cur_note
    clc
    adc #15
    sta cur_note
parse_abc15:
  lda cur_note
  clc
  adc #15
  sta cur_note
  jsr abc_taste_char
  cmp #$27 ; '''
  bne @L1
  jsr abc_read_char
  lda #15
  clc
  adc cur_note
  sta cur_note
  bra @L3
@L1:
  cmp #','
  bne @L3
  jsr abc_read_char
  lda cur_note
  sec
  sbc #15
  sta cur_note
@L3:
  lda cur_note
  phy
  tay
  lda _delays,y
  phx
  pha
  txa
  clc
  rol
  clc
  rol
  clc
  rol
  tax
  pla
  sta $fd24,x
  plx
  ply
  ; Find out the volume of the note
  lda abc_note_volume,x
set_music_ptr:
  sta sound_channel_max_volume,x
  ; Find out the duration of the note
  jsr abc_read_number
  pha
  lda abc_tmp
  bne @L1
  ina
@L1:
  phy
  tay
  lda #0
@L2:
  clc
  adc abc_note_length,x
  dey
  bne @L2
  sta sound_channel_duration,x
  ply
  pla
  tya
  sta _abc_music_ptr,x
  lda abc_instrument_maxlen,x
  sta sound_channel_maxlen,x
  rts

; Read a character from the score. Advance ptr if it is not 0
; X - channel
; Y - score offset
abc_read_char:
  txa
  ina
  dea
  bne @L33
    lda (_abc_score_ptr0),y
    sta abc_tmp+2
    bne @L31
    rts
@L31:
    iny
    bne @L32
      inc _abc_score_ptr0+1
@L32:
    rts
@L33:
  dea
  bne @L36
    lda (_abc_score_ptr1),y
    sta abc_tmp+2
    bne @L34
    rts
@L34:
    iny
    bne @L35
      inc _abc_score_ptr1+1
@L35:
    rts
@L36:
  dea
  bne @L39
    lda (_abc_score_ptr2),y
    sta abc_tmp+2
    bne @L37
    rts
@L37:
    iny
    bne @L38
      inc _abc_score_ptr2+1
@L38:
    rts
@L39:
    lda (_abc_score_ptr3),y
    sta abc_tmp+2
    bne @L310
    rts
@L310:
    iny
    bne @L311
      inc _abc_score_ptr3+1
@L311:
    rts

; Read a character from the score.
; X - channel
; Y - score offset
abc_taste_char:
  txa
  ina
  dea
  bne @L53
    lda (_abc_score_ptr0),y
    sta abc_tmp+2
    rts
@L53:
  dea
  bne @L56
    lda (_abc_score_ptr1),y
    sta abc_tmp+2
    rts
@L56:
  dea
  bne @L59
    lda (_abc_score_ptr2),y
    sta abc_tmp+2
    rts
@L59:
    lda (_abc_score_ptr3),y
    sta abc_tmp+2
    rts

; Instrument setup. C-interface
_abcinstrument:
  ; Read the channel number
  jsr popa
  sta channel_nr
  clc
  rol
  clc
  rol
  clc
  rol
  sta channel_offs

  ; Disable count
  lda $fd25,x
  and #$f7
  sta $fd25,x

  ; Read byte containing taps register bits 012345 10 11
  jsr popa
  ldx channel_offs
  sta $fd21,x

  ; $fd22,x contains the audio output value
  
  ; Read byte containing backup register bits 01234567
  jsr popa
  ldx channel_offs
  sta $fd23,x

  ; $fd24,x contains the audio timer backup value = note

  ; Read byte containing backup register bits 89 10 11
  jsr popa
  ldx channel_offs
  sta $fd27,x

  ; Read audio control byte bits octave 012 integrate 5 taps 7
  ; This also enables counts again
  jsr popa
  ldx channel_offs
  sta $fd25,x

  ; Enable count
  lda $fd25,x
  ora #$08
  sta $fd25,x

  ; Read tempo
  jsr popa
  ldx channel_nr
  sta abc_note_length,x

  ; Read volume
  jsr popa
  ldx channel_nr
  sta abc_note_volume,x

  ; Read attack
  jsr popa
  ldx channel_nr
  sta abc_instrument_incr,x

  ; Read hold
  jsr popa
  ldx channel_nr
  sta abc_instrument_maxlen,x

  ; Read decay
  jsr popa
  ldx channel_nr
  sta abc_instrument_decr,x
  rts

	.data
abcmetronome: .res 2
instrumenttmp: .res 2
channel_offs: .res 1
channel_nr: .res 1
_abc_music_ptr:   .byte 0,0,0,0
abc_repeat_offs: .byte 0,0,0,0
abc_repeat_hoffs: .byte 0,0,0,0
abc_repeat_cnt:  .byte 0,0,0,0
sound_channel_duration: .byte 0,0,0,0
abc_note_length: .byte 6,6,6,6
abc_note_volume: .byte 64,64,64,64
abc_instrument_incr:   .byte 4,4,4,4
abc_instrument_maxlen: .byte 4,4,4,4
abc_instrument_decr:   .byte 4,4,4,4
sound_channel_max_volume: .byte 60,127,127,127
sound_channel_volume:   .byte 4,4,4,4
sound_channel_maxlen:   .byte 4,4,4,4
cur_note:    .byte 0
abc_tmp: .res 3

	.rodata
; This table is used to cover the delays needed for 4 octaves
; These values work when the looplen is 2, 4, 8, 16...
_delays: .byte 151 ; _A,
        .byte 142 ; A,
        .byte 134 ; ^A, _B,
        .byte 127 ; B,
        .byte 119 ;
        .byte 239 ; C,
        .byte 225 ; ^C, _D,
        .byte 213 ; D,
        .byte 201 ; ^D, _E,
        .byte 190 ; E,
        .byte 179 ;
        .byte 179 ; F,
        .byte 169 ; ^F, _G,
        .byte 159 ; G,
        .byte 151 ; ^G,

        .byte  75 ; _A
        .byte  71 ; A
        .byte  67 ; ^A _B
        .byte  63 ; B
        .byte  60 ;
        .byte 119 ; C
        .byte 113 ; ^C _D
        .byte 106 ; D
        .byte 100 ; ^D _E
        .byte  95 ; E
        .byte  89 ;
        .byte  89 ; F
        .byte  84 ; ^F _G
        .byte  80 ; G
        .byte  75 ; ^G

        .byte  38 ; _a
        .byte  36 ; a
        .byte  34 ; ^a _b
        .byte  32 ; b
        .byte  30 ;
        .byte  60 ; c
        .byte  56 ; ^c _d
        .byte  53 ; d
        .byte  50 ; ^d _e
        .byte  47 ; e
        .byte  45 ;
        .byte  45 ; f
        .byte  42 ; ^f _g
        .byte  40 ; g
        .byte  38 ; ^g

        .byte  19 ; _a'
        .byte  18 ; a'
        .byte  17 ; ^a' _b'
        .byte  16 ; b'
        .byte  15 ;
        .byte  30 ; c'
        .byte  28 ; ^c' _d'
        .byte  27 ; d'
        .byte  25 ; ^d' _e'
        .byte  24 ; e'
        .byte  22 ;
        .byte  22 ; f'
        .byte  21 ; ^f' _g'
        .byte  20 ; g'
        .byte  19 ; ^g'

