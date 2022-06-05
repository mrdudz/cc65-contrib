/*
   Copyright (c) 2003, Swen Moczarski
   All rights reserved.

   Redistribution and use in source and binary forms, with or without 
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, 
     this list of conditions and the following disclaimer.
  
   * Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the documentation 
     and/or other materials provided with the distribution.
  
   * Neither the name of the copyright holders nor the names of its contributors 
     may be used to endorse or promote products derived from this software 
     without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
   POSSIBILITY OF SUCH DAMAGE.
*/

#include <avr/io.h>


//  EXPORTS - Functions
    .global  p_pos_startFirstContext
    .global  p_pos_softContextSwitch
    .global  p_pos_intContextSwitch
    .global  p_pos_findbit

//  IMPORTS - Functions
    .extern  c_pos_intEnter
    .extern  c_pos_intExit
    .extern  c_pos_timerInterrupt

//  IMPORTS - Variables
    .extern  posCurrentTask_g
    .extern  posNextTask_g
    .extern  posInInterrupt_g

.macro PUSH_GPRS
        push   r0
		push   r1
        push   r2
        push   r3
        push   r4
        push   r5
        push   r6
        push   r7
        push   r8
        push   r9
        push   r10
        push   r11
        push   r12
        push   r13
        push   r14
        push   r15
        push   r16
        push   r17
        push   r18
        push   r19
        push   r20
        push   r21
        push   r22
        push   r23
        push   r24
        push   r25
        push   r26
        push   r27
        push   r28
        push   r29
        push   r30
        push   r31
        in     r0, _SFR_IO_ADDR(SREG)
        push   r0
.endm


.macro POP_GPRS
        pop    r0
        out    _SFR_IO_ADDR(SREG), r0
        pop    r31
        pop    r30
        pop    r29
        pop    r28
        pop    r27
        pop    r26
        pop    r25
        pop    r24
        pop    r23
        pop    r22
        pop    r21
        pop    r20
        pop    r19
        pop    r18
        pop    r17
        pop    r16
        pop    r15
        pop    r14
        pop    r13
        pop    r12
        pop    r11
        pop    r10
        pop    r9
        pop    r8
        pop    r7
        pop    r6
        pop    r5
        pop    r4
        pop    r3
        pop    r2
        pop    r1
        pop    r0
.endm


// SP = posCurrentTask_g->stackptr;
.macro LOAD_SP
        lds    XL, posCurrentTask_g
        lds    XH, posCurrentTask_g + 1
        
        ld     r18, x+
        out    _SFR_IO_ADDR(SPL), r18
        ld     r18, x
        out    _SFR_IO_ADDR(SPH), r18
.endm



// posCurrentTask_g->stackptr = SP;
.macro STORE_SP
        lds    XL, posCurrentTask_g
        lds    XH, posCurrentTask_g + 1
        
        in     r18, _SFR_IO_ADDR(SPL)
        st     x+, r18
        in     r18, _SFR_IO_ADDR(SPH)
        st     x, r18
.endm


        
/**
 * Responsibilities:
 *
 *  1. save current task context:
 *        - save General Purpose Register and SREG 
 *          (only for p_pos_softContextSwitch!)
 *        - save stack pointer
 *
 *  2. switch current task context to the next
 *        posCurrentTask_g = posNextTask_g;
 *
 *  3. restore context of the next task to execute:
 *        - restore stack pointer
 *        - restore General Purpose Register and SREG
 */
p_pos_softContextSwitch:

        PUSH_GPRS

        STORE_SP

p_pos_intContextSwitch:

        // posCurrentTask_g = posNextTask_g;
        lds    r18, posNextTask_g
        sts    posCurrentTask_g, r18
        
        lds    r18, posNextTask_g + 1
        sts    posCurrentTask_g + 1, r18

p_pos_startFirstContext:

        LOAD_SP


interruptReturn:

        POP_GPRS

        reti
        
        
        

// r24 = bitfield
// r22 = rrOffset
p_pos_findbit:
         mov    r18, r24
         mov    r19, r22

         inc    r19
         clc
offsetLoop:
         ror    r18
         dec    r19
         brne   offsetLoop
         
         mov    r19, r22
nextBitLoop:
         brcs   bitFound
         ror    r18
         inc    r19
         rjmp   nextBitLoop
         
bitFound:
         mov    r24, r19
         subi   r24, 0x08
         
         brpl   correctOffset
         mov    r24, r19
         ret
         
correctOffset:  
         dec    r24
         ret
        

/**
 * This interrupt is used to generate the pico[OS time slices.
 *
 * Responsibilities:
 *
 *   1. save current task context:
 *        - save General Purpose Register and SREG 
 *
 *   2. if (posInInterrupt_g == 0), save stack pointer to current task environment
 *
 *   3. call to c_pos_intEnter()
 *
 *   4. call to c_pos_timerInterrupt()
 *
 *   5. a user finction can be called here
 *
 *   6. call to  c_pos_intExit()
 *
 *   7. restore General Purpose Register and SREG
 */
 
.global SIG_OUTPUT_COMPARE1A
SIG_OUTPUT_COMPARE1A:

        PUSH_GPRS

        // if (posInInterrupt_g == 0)  posCurrentTask_g->stackptr = SP;
        lds    r18, posInInterrupt_g
        tst    r18
        brne   interruptRun

        STORE_SP
        
interruptRun:
        call   c_pos_intEnter
        call   c_pos_timerInterrupt
        call   c_pos_intExit
        
        rjmp   interruptReturn
        
