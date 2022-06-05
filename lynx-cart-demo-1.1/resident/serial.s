; TIMER4 = UART-Timer
contrl          = %00011101

Baudrate        = 9615

prescale        = 13 ; 125000/Baudrate-1

.bss
TxBuffer: .res 256
RxBuffer: .res 256

TxBufferSize	= 256
RxBufferSize	= 256
TimeOutTimer	= 7

RxParityErr	= %00010000
RxOverrun	= %00001000
RxFrameErr	= %00000100
RxBreak	= %00000010

.segment "MYZP" : zp
TxPtrIn:	.res 1
TxPtrOut:	.res 1
RxPtrIn:	.res 1
RxPtrOut:	.res 1

LastSerialStat:	.res 1
SerialStat:	.res 1	; ComLynx-Errors $FD8C(r)
SerialContrl:	.res 1	; copy of $FD8C(w)
TimeOutFlag:	.res 1	; <0 => TimeOut
TxDone:	.res 1	; <0 last char send
SerialLastRxTime: .res 2	; time of last char received
SerialTimeOut:	.res 1	; time for TimeOut

.code
InitComLynx:
                php
                sei

patch0:          ; IF prescale<256
                lda #prescale
                sta $fd10
                lda #%00011000
               ; ELSE
               ; lda #prescale/2
               ; sta $fd10
               ; lda #%00011001
               ; ENDIF
                sta $fd11

                lda #contrl
                sta SerialContrl
                ora #$48
                sta $fd8c
                stz TxDone
                stz TxPtrIn
                stz TxPtrOut
                stz RxPtrIn
                stz RxPtrOut

@exit:          ;SETIRQVEC 4,SerialIRQ
                plp
                rts
SerialIRQ:
                bit TxDone
                bmi @tx_irq
                ldx $fd8d
                lda $fd8c
                and #RxParityErr|RxOverrun|RxFrameErr|RxBreak
                beq @rx_irq
                tsb SerialStat  ; ev. Fehler merken
                bit #RxBreak
                beq @noBreak
                stz TxPtrIn
                stz TxPtrOut
                stz RxPtrIn
                stz RxPtrOut
@noBreak:       lda #$48|contrl        ; RxIRQ setzen und Fehler l”schen
                sta $fd8c
                lda #$10
                sta $fd80
                rti

@rx_irq:        lda #$48|contrl        ; RxIRQ setzen und Fehler l”schen
                sta $fd8c
                txa
                ldx RxPtrIn
                sta RxBuffer,x
                txa
                inx

@cont0:         cpx RxPtrOut
                beq @1
                stx RxPtrIn
                lda #$10
                sta $fd80
                rti

@1:             sta RxPtrIn
                lda #$80
                tsb SerialStat

@exit0:
                lda #$10
                sta $fd80
                rti

@tx_irq:        ldx TxPtrOut
                cpx TxPtrIn
                beq @cont1

                lda TxBuffer,x
                sta $fd8d

                inc TxPtrOut

@exit1:         lda #$88|contrl
                sta $fd8c
                lda #$10
                sta $fd80
                rti

@cont1:         lda $fd8c
                bit #$20
                beq @exit1
                bvs @exit1
                stz TxDone
                lda #$48|contrl
                sta $fd8c

                lda #$10
                sta $fd80
                rti

RecComLynxByte:
                phx
                ldx RxPtrOut
                cpx RxPtrIn
                beq @99
                lda RxBuffer,x
                inc RxPtrOut
                plx
                sec
                rts
@99:            plx
                clc
                rts

SndComLynxByte:
                phx
                ldx TxPtrIn
                sta TxBuffer,x
                inx
                cpx TxPtrOut
                beq @error
                stx TxPtrIn
                bit TxDone
                bmi @L1
                  php
                  sei
                  lda #$88|contrl
                  sta $fd8c       ; TX-IRQ erlauben RX-IRQ sperren
                  sta TxDone
                  plp
@L1:
                plx
                sec
                rts

@error:         plx
                clc
                rts

WaitComLynxByte:
                stz SerialStat
@loop:          jsr RecComLynxByte
                bcs @exit
                lda SerialStat
                beq @loop

@exit0:         lda #0
                clc
@exit:          rts

TxStat:
; C = 0 => send buffer full
                lda TxPtrIn
                cmp TxPtrOut
                beq @exit
                sec
@exit:          rts
