/*
 *  Copyright (c) 2004, Dennis Kuschel.
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


/**
 * @file   n_core.c
 * @brief  nano layer, core file
 * @author Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: n_core.c,v 1.2 2004/03/21 18:34:38 dkuschel Exp $
 */

#define _N_CORE_C
#include "../src/nano/privnano.h"

/* check features */
#if NOS_FEATURE_CPUUSAGE != 0
#if POSCFG_FEATURE_SLEEP == 0
#error POSCFG_FEATURE_SLEEP not enabled
#endif
#if POSCFG_FEATURE_IDLETASKHOOK == 0
#error POSCFG_FEATURE_IDLETASKHOOK not enabled
#endif
#endif
#if POSCFG_TASKSTACKTYPE == 0
#if NOSCFG_FEATURE_MEMALLOC == 0
#error NOSCFG_FEATURE_MEMALLOC not enabled
#endif
#endif



/*---------------------------------------------------------------------------
 *  PROTOTYPES, TYPEDEFS AND VARIABLES
 *-------------------------------------------------------------------------*/

static void nano_init(void *arg);

#if NOS_FEATURE_CPUUSAGE != 0

static void nano_idlehook(void);
static void nano_initCpuUsage(void);

static unsigned long  idle_counter_g = 0;
static unsigned long  idle_loops_g;
static unsigned long  idle_loops_100p_g;
static UVAR_t         idle_predivide_g;
static JIF_t          idle_jiffies_g = 0;

#define IDLE_MULBITS     11  /* = 2048;  2048 / 20 = 102.4 ~ 100% */
#define IDLE_PREDIVIDER  (1<<IDLE_MULBITS)
#endif

struct {
  POSTASKFUNC_t  func;
  void           *arg;
} taskparams_g;



/*---------------------------------------------------------------------------
 *  CPU USAGE MEASUREMENT
 *-------------------------------------------------------------------------*/

#if NOS_FEATURE_CPUUSAGE != 0

static void nano_idlehook(void)
{
  JIF_t jif;
  POS_LOCKFLAGS;

  jif = jiffies;
  POS_SCHED_LOCK;
  if (POS_TIMEAFTER(jif, idle_jiffies_g))
  {
    idle_loops_g   = idle_counter_g;
    idle_jiffies_g = jif + HZ;
    idle_counter_g = 0;
  }
  else
  {
    idle_counter_g++;
  }
  POS_SCHED_UNLOCK;
}

/*-------------------------------------------------------------------------*/

static void nano_initCpuUsage(void)
{
  (void) posInstallIdleTaskHook(nano_idlehook);
  posTaskSleep(1);
  idle_jiffies_g = jiffies + HZ;
  idle_counter_g = 0;
  posTaskSleep(HZ + 2);
  idle_loops_100p_g = idle_loops_g;
  if (idle_loops_100p_g == 0)
    idle_loops_100p_g = 1;
  idle_predivide_g = 0;
  if (idle_loops_100p_g > 1L*(IDLE_PREDIVIDER << (31-(2*IDLE_MULBITS))))
  {
    idle_predivide_g = 1;
    idle_loops_100p_g /= IDLE_PREDIVIDER;
  }
}

/*-------------------------------------------------------------------------*/

UVAR_t nosCpuUsage(void)
{
  unsigned long tmp;
  UINT_t  p;
  JIF_t   jif;
  POS_LOCKFLAGS;

  jif = jiffies;
  POS_SCHED_LOCK;
  tmp = idle_loops_g;
  if (POS_TIMEAFTER(jif, idle_jiffies_g + HZ))
  {
    idle_jiffies_g = jif + HZ;
    idle_counter_g = 0;
    idle_loops_g   = 0;
  }
  POS_SCHED_UNLOCK;
  if (idle_predivide_g != 0)
    tmp /= IDLE_PREDIVIDER;
  tmp = idle_loops_100p_g - tmp;
  if ((long)tmp < 0) 
    return 0;
  p = ((UINT_t) ((tmp << IDLE_MULBITS) / idle_loops_100p_g)) / 20;
  if (p > 100)  p = 100;
  return (UVAR_t) p;
}

#endif /* NOS_FEATURE_CPUUSAGE */



/*---------------------------------------------------------------------------
 *  NANO LAYER TASK CREATION
 *-------------------------------------------------------------------------*/

#if POSCFG_TASKSTACKTYPE == 0

void nos_taskMemFree(POSTASK_t task);
void nos_taskMemFree(POSTASK_t task)
{
  NOS_MEM_FREE(task->nosstkroot);
}

#endif /* POSCFG_TASKSTACKTYPE == 0 */

/*-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_TASKCREATE != 0

POSTASK_t nosTaskCreate(POSTASKFUNC_t funcptr, void *funcarg,
                        VAR_t priority, UINT_t stacksize,
                        const char* name)
{
  POSTASK_t task;

#if POSCFG_TASKSTACKTYPE == 0
  void *stk;

  if (stacksize == 0)
    stacksize = NOSCFG_DEFAULT_STACKSIZE;

  stk = nosMemAlloc(NOSCFG_STKMEM_RESERVE + stacksize);
  if (stk == NULL)
    return NULL;

#if POSCFG_FEATURE_INHIBITSCHED == 0
#error POSCFG_FEATURE_INHIBITSCHED not enabled
#endif
  posTaskSchedLock();
#if NOSCFG_STACK_GROWS_UP == 0
  task = posTaskCreate(funcptr, funcarg, priority, 
                       (void*) (((MEMPTR_t)stk) + stacksize -
                                (NOSCFG_STKMEM_RESERVE + 1)));
#else
  task = posTaskCreate(funcptr, funcarg, priority,
                       (void*) (((MEMPTR_t)stk) + NOSCFG_STKMEM_RESERVE));
#endif
  if (task != NULL)
  {
    task->nosstkroot = stk;
    task->stkfree = nos_taskMemFree;
  }
  posTaskSchedUnlock();
#if POSCFG_FEATURE_SLEEP != 0
  posTaskSleep(0);
#elif POSCFG_FEATURE_YIELD != 0
  posTaskYield();
#endif

#elif POSCFG_TASKSTACKTYPE == 1

  if (stacksize == 0)
    stacksize = NOSCFG_DEFAULT_STACKSIZE;

  task = posTaskCreate(funcptr, funcarg, priority, stacksize);

#elif POSCFG_TASKSTACKTYPE == 2

  (void) stacksize;
  task = posTaskCreate(funcptr, funcarg, priority);

#endif /* POSCFG_TASKSTACKTYPE == 2 */

  /* named tasks are not yet supported */
  (void) name;

  return task;
}

#endif /* NOSCFG_FEATURE_TASKCREATE != 0 */



/*---------------------------------------------------------------------------
 *  NANO LAYER INITIALIZATION
 *-------------------------------------------------------------------------*/

static void nano_init(void *arg)
{
#if POSCFG_TASKSTACKTYPE == 0
  posCurrentTask_g->nosstkroot = arg;
  posCurrentTask_g->stkfree = nos_taskMemFree;
#else
  (void) arg;
#endif

#if NOS_FEATURE_CPUUSAGE != 0
  nano_initCpuUsage();
#endif
#if (NOSCFG_FEATURE_CONIN != 0) || (NOSCFG_FEATURE_CONOUT != 0)
  nos_initConIO();
#endif
#if NOSCFG_FEATURE_BOTTOMHALF != 0
  nos_initBottomHalfs();
#endif

  (taskparams_g.func)(taskparams_g.arg);
}

/*-------------------------------------------------------------------------*/

void  nosInit(POSTASKFUNC_t firstfunc, void *funcarg, VAR_t priority,
              UINT_t taskStackSize, UINT_t idleStackSize)
{
#if POSCFG_TASKSTACKTYPE == 0
  void *stk_task1, *stk_idle;
#endif

  taskparams_g.func = firstfunc;
  taskparams_g.arg  = funcarg;

#if (NOSCFG_FEATURE_MEMALLOC != 0) && (NOSCFG_MEM_MANAGER_TYPE == 1)
  nos_initMem();
#endif

#if POSCFG_TASKSTACKTYPE == 0

  if (taskStackSize == 0)
    taskStackSize = NOSCFG_DEFAULT_STACKSIZE;
  if (idleStackSize == 0)
    idleStackSize = NOSCFG_DEFAULT_STACKSIZE;
  stk_task1 = NOS_MEM_ALLOC(NOSCFG_STKMEM_RESERVE + taskStackSize);
  stk_idle  = NOS_MEM_ALLOC(NOSCFG_STKMEM_RESERVE + idleStackSize);
  if ((stk_task1 != NULL) && (stk_idle != NULL))
  {
#if NOSCFG_STACK_GROWS_UP == 0
    posInit(nano_init, stk_task1, priority,
            (void*) (((MEMPTR_t)stk_task1) + taskStackSize -
                     (NOSCFG_STKMEM_RESERVE + 1)),
            (void*) (((MEMPTR_t)stk_idle) + idleStackSize -
                     (NOSCFG_STKMEM_RESERVE + 1)) );
#else
    posInit(nano_init, stk_task1, priority,
            (void*) (((MEMPTR_t)stk_task1) + NOSCFG_STKMEM_RESERVE),
            (void*) (((MEMPTR_t)stk_idle) + NOSCFG_STKMEM_RESERVE) );
#endif
  }

#elif POSCFG_TASKSTACKTYPE == 1

  if (taskStackSize == 0)
    taskStackSize = NOSCFG_DEFAULT_STACKSIZE;
  if (idleStackSize == 0)
    idleStackSize = NOSCFG_DEFAULT_STACKSIZE;
  posInit(nano_init, NULL, priority, taskStackSize, idleStackSize);

#elif POSCFG_TASKSTACKTYPE == 2

  (void) taskStackSize;
  (void) idleStackSize;
  posInit(nano_init, NULL, priority);

#endif
}
