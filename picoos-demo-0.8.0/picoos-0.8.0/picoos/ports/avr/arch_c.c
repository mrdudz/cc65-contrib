/*
 *  Copyright (c) 2004, Swen Moczarski, Dennis Kuschel.
 *  All rights reserved. 
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission. 
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 *  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


/*
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: arch_c.c,v 1.1.1.1 2004/02/16 20:11:28 smocz Exp $
 */

#include <inttypes.h>
#include <avr/io.h>

#include "picoos.h"

#include "timerdef.h"


/* local prototypes */
static uint8_t* putPointerOnStack(uint8_t* stackPtr, void* functionPointer);
static void constructStackFrame(POSTASK_t task, uint8_t* stackPtr, POSTASKFUNC_t funcptr, void *funcarg);


// The initial value of SREG at the beginning of the task.
// The flag Global Interrupt Enable must be enabled so that 
// the timer interrupt can work.
#define INITIAL_SREG _BV(SREG_I)

// the amount of general purpose register in the 
// AVR-Architecture to be saved in the context switch
#define GP_REGISTER_AMOUNT 32

// The number of the gp-register, for passing the argument
// to the called function. In the current gcc implementation (gcc 3.3)
// is this R25:R24 for an pointer
#define ARGUMENT_REGISTER_NUM 24


/*
 * Wrap malloc() and free() from the standard c-lib function for 
 * a thread save behavior.
 */
#if (POSCFG_TASKSTACKTYPE == 1)

void  mem_init(void);
void* malloc(int size);
void  free(void *p);



void mem_init(void) {

}


void* my_malloc(UINT_t size) {
    POS_SCHED_LOCK;
    // TODO: call the malloc() from avr-lib here?
    POS_SCHED_UNLOCK;
}


void my_free(void *p) {
    POS_SCHED_LOCK;
    // TODO: call the free() from avr-lib here?
    POS_SCHED_UNLOCK;
}

#endif



/*
 * Initialize the port. 
 * 
 * The timer for the scheduler will be started here.
 */
/*void p_pos_initArch(void) {
  
    POS_SCHED_LOCK;

    // TODO: consistent-check with #error possible? 
    // CRYSTAL_CLOCK / (TIMER_PRESCALER_VALUE * HZ) < 65536
    uint16_t outputCompare = CRYSTAL_CLOCK / (TIMER_PRESCALER_VALUE * HZ);
    
    OCR1A = outputCompare;
    TCCR1A = 0x00;
    // set WGM12 for Clear Timer on Compare match (CTC) mode
    TCCR1B = _BV(WGM12) | TIMER_PRESCALER_FLAGS;
    TIMSK |= _BV(OCIE1A);   // enable interrupt

    POS_SCHED_UNLOCK;
}*/

void p_pos_initArch(void) {
  
    // TODO: Globale Interrupts abschalten
    POS_SCHED_LOCK;

    TIMER_COUNTER_REG = TIMER_COUNTER_VALUE;
    
    // TODO: need we that ?
    // TCCR1A = 0x00;
    
    TIMER_CONFIG_REG = TIMER_CONFIG_VALUE;
    TIMER_INTERRUPT_REG |= _BV(TIMER_INTERRUPT_ENABLE_BIT);   // enable interrupt

    POS_SCHED_UNLOCK;
}

/*---------------------------------------------------------------------------
 * CONTEXT SWITCH FUNCTIONS  (implemented in assembler, see arch_a.asm)
 *-------------------------------------------------------------------------*/

#if 0
void p_pos_startFirstContext(void) {
  /* TO DO:  restore new task data from new task control block */
}

void p_pos_softContextSwitch(void) {
  /* TO DO:  save old task data to current task control block */
  
  /* set new task */
  posCurrentTask_g = posNextTask_g;
  
  /* TO DO:  restore new task data from new task control block */
}

void p_pos_intContextSwitch(void) {
  /* TO DO:  save old task data to current task control block */
  
  /* set new task */
  posCurrentTask_g = posNextTask_g;
  
  /* TO DO:  restore new task data from new task control block */
}
#endif



/*---------------------------------------------------------------------------
 * INIT TASK CONTEXT  (and allocate stack memory if supposed)
 *-------------------------------------------------------------------------*/

#if (POSCFG_TASKSTACKTYPE == 0)

void p_pos_initTask(POSTASK_t task, 
                   void *user,
                   POSTASKFUNC_t funcptr, 
                   void *funcarg) {
                       
    uint8_t *stackPtr = (uint8_t*)user;
    constructStackFrame(task, stackPtr, funcptr, funcarg);
}


#elif (POSCFG_TASKSTACKTYPE == 1)


VAR_t p_pos_initTask(POSTASK_t task, 
                    UINT_t stacksize,
                    POSTASKFUNC_t funcptr, 
                    void *funcarg) {
                      
    uint8_t *stackPtr = my_malloc(stacksize);
    
    if (stackPtr == NULL) {
        return -1;
    }
    task->stackroot = stackPtr;
    stackPtr += stacksize - 1;

    constructStackFrame(task, stackPtr, funcptr, funcarg);

    return 0;
}


void  p_pos_freeStack(POSTASK_t task) {
    my_free(task->stackroot);
}


#elif (POSCFG_TASKSTACKTYPE == 2)


VAR_t p_pos_initTask(POSTASK_t task,
                    POSTASKFUNC_t funcptr, 
                    void *funcarg) {
    // Stack pointer is in the stack memory 
    // (with fixed size) in the task structure.
    //uint8_t *stackPtr = &(task->stack[FIXED_STACK_SIZE-1]);
    uint8_t *stackPtr = (task->stack) + (FIXED_STACK_SIZE-1);

    constructStackFrame(task, stackPtr, funcptr, funcarg);

    return 0;
}

void  p_pos_freeStack(POSTASK_t task) {
  /* this function is called but not needed */
  (void) task;
}


#else
#error "Error in configuration for the port (postcfg.h): POSCFG_TASKSTACKTYPE must be 0, 1 or 2"
#endif

/**
 * The stack frame are construct as follow:
 *                           value       position
 *   stackPtr(begin) -> [ funcarc      ] [-0 ]
 *                      [ exitFunction ] [-2 ] Note: posTaskExit or exitFeatureDisabeldAlert
 *                      [ funcptr      ] [-4 ]
 *                      [ SREG         ] [-6 ]
 *                      [ R0 .. R31    ] [-7 ]
 *   stackPtr(end)   -> [ undefine     ] [-39]
 */
void constructStackFrame(POSTASK_t task, uint8_t* stackPtr, 
                             POSTASKFUNC_t funcptr, 
                             void *funcarg) {

#if (POSCFG_FEATURE_EXIT != 0)
    // put the pointer for the exit function on the stack frame
    stackPtr = putPointerOnStack(stackPtr, posTaskExit);
#else
    // the call of this function shouldd not happen
    // TODO: put an function for warning here?
    stackPtr = putPointerOnStack(stackPtr, funcptr);
#endif
    stackPtr = putPointerOnStack(stackPtr, funcptr);
    
    // Initialize the register from R0 to the register, 
    // that pass the argument with 0x00

    uint8_t* registerStartPtr;

    for (registerStartPtr = stackPtr; 
         stackPtr > (registerStartPtr - ARGUMENT_REGISTER_NUM); 
         stackPtr--) {
            
        *stackPtr = 0;
    }
    // Put funcarg of the stack. If the context will be start the 
    // first time, the argument will be loaded in apropriated
    // register and is interpreted as a pointer parameter.
    stackPtr = putPointerOnStack(stackPtr, funcarg);
    
    // Now fill the remaining gp-registers with 0.
    for (; stackPtr > (registerStartPtr - GP_REGISTER_AMOUNT); stackPtr--) {
        *stackPtr = 0;
    }

    // setup the stack 
    *stackPtr = INITIAL_SREG;
    stackPtr--;
    
    task->stackptr = (void*) stackPtr;
}


/**
 * Helper to put a 16-bit pointer on the stack.
 * 
 * @pre
 *  stackPtr ->   [stack - 0] x
 *                [stack - 1] x
 *                [stack - 2] x
 * 
 * @post
 *                [stack - 0] (lo)pointer
 *                [stack - 1] (hi)pointer
 *  stackPtr ->   [stack - 2] x
 * 
 * @param stackPtr The pointer to the next _free_ position on the stack
 * @param pointer The pointer 
 * 
 * @return The modified pointer to the next _free_ position on the stack
 */
uint8_t* putPointerOnStack(uint8_t* stackPtr, void* pointer) {
    uint16_t ptrWord = (uint16_t)((void*)pointer);
    
    uint8_t lo = (uint8_t)(ptrWord & 0x00FF);
    uint8_t hi = (uint8_t)((ptrWord >> 8) & 0x00FF);
    
    *stackPtr = lo;
    stackPtr--;
    *stackPtr = hi;
    stackPtr--;
    
    return stackPtr;
}
