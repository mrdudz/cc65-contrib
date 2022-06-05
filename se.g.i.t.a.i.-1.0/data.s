.SEGMENT "MUSIC"
_music_init:
_music_play=*+3
.INCBIN "datafiles/Intro.mus"

.EXPORT _music_init, _music_play

.SEGMENT "INTROCHARSET"
.INCBIN "datafiles/1x2charset_hires.prg"

.SEGMENT "FADETAB"
.SEGMENT "FADECODE1"
_fadecode1:
.EXPORT _fadecode1
.SEGMENT "FADECODE2"
_fadecode2:
.EXPORT _fadecode2

.SEGMENT "LOGOBITMAP1"
.INCBIN "datafiles/logo.bmp"
.SEGMENT "LOGOBITMAP2"

.SEGMENT "FRAMEBITMAP1"

.SEGMENT "LOGOSCRN1"
.INCBIN "datafiles/logo.scr"


.SEGMENT "LOGOBITMAP2"
.SEGMENT "FRAMEBITMAP2"

.SEGMENT "LOGOSCRN2"

.DATA
_logocolors:
.EXPORT _logocolors
.INCBIN "datafiles/logo.col"

.SEGMENT "GHOSTBYTE"
_ghostbyte:
.EXPORT _ghostbyte

.SEGMENT "PART2DEST"
.SEGMENT "PART2STARTUP"
.SEGMENT "PART2"
.INCBIN "mountains/paralax.exo"

.SEGMENT "HEAP"
