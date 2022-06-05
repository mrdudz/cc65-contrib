.ifndef z_zpvar
.define z_zpvar

.define z_cpu 1
.define z_tmp0 2
.define z_ftoiptr 3
.define z_itofptr 5
.define z_searchchr 7
.define z_endchr 8
.define z_tabx	9
.define z_loadverflag 10
.define z_inputbuflen 11
.define z_dimflag 12
.define z_valtype 13
.define z_intflag 14
.define z_garbflag 15
.define z_usrflag 16
.define z_inputflag 17
.define z_tansign 18
.define z_inputcomment 19
.define z_linnumber $14
z_tmp1 = $16
z_laststradr = $17
z_12tmp = $19

.define z_localzp  $19

.define z_startbas  $2b
.define z_startvar  $2d
.define z_startarray $2f

.define z_curbaslin  $39
.define z_prevbaslin  $3b
.define z_contlin  $3d
.define z_datalin  $3f
.define z_dataptr  $41

.define z_inputvec  $43

.define z_varname  $45
.define z_varpnt  $47
.define z_forpnt  $49

.define z_facexp  $61
.define z_facman  $62
.define z_facsign  $66

;.define z_signflag  $67

.define z_status    $90

.define z_ptr1 $ae
.define z_ptr2 $b0

.define z_drivenum  $ba

.define z_crxpos    $d3
.define z_crypos    $d6

.define z_5tmp $fb ;five tmp zp places

.endif


