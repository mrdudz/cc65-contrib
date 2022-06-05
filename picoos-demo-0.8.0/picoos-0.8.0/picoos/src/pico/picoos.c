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
 * @file   picoos.c
 * @brief  pico]OS core file
 * @author Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: picoos.c,v 1.7 2004/03/15 21:09:22 dkuschel Exp $
 */


#define _POSCORE_C
#define PICOS_PRIVINCL
#include <picoos.h>



/*---------------------------------------------------------------------------
 *  MACROS FOR MEMORY ALIGNMENT
 *-------------------------------------------------------------------------*/

#if POSCFG_ALIGNMENT < 2
#define ALIGNEDSIZE(size)               (size)
#define ALIGNEDBUFSIZE(elemsize, count) ((elemsize)*(count))
#define MEMALIGN(type, var)             (type)((void*)(var))
#define NEXTALIGNED(type, var)          (((type)((void*)(var))) + 1)
#else
#define ALIGNEDSIZE(size) \
  (((size) + (POSCFG_ALIGNMENT - 1)) & ~(POSCFG_ALIGNMENT - 1))
#define ALIGNEDBUFSIZE(elemsize, count) \
  (ALIGNEDSIZE(elemsize)*(count) + (POSCFG_ALIGNMENT - 1))
#define MEMALIGN(type, var)  (type)((void*) \
  ((((MEMPTR_t)(var))+POSCFG_ALIGNMENT-1)&~(POSCFG_ALIGNMENT-1)))
#define NEXTALIGNED(type, var) \
  (type)((void*)(((MEMPTR_t)(var)) + ALIGNEDSIZE(sizeof(*(var)))))
#endif  /* POSCFG_ALIGNMENT */

#define STATICBUFFER(glblvar, size, count)  static UVAR_t \
  glblvar[((ALIGNEDSIZE(size)*count) + POSCFG_ALIGNMENT + sizeof(UVAR_t)-2) \
          / sizeof(UVAR_t)]



/*---------------------------------------------------------------------------
 *  LOCAL TYPES AND VARIABLES
 *-------------------------------------------------------------------------*/

typedef struct TBITS {
  UVAR_t         xtable[SYS_TASKTABSIZE_Y];
#if SYS_TASKTABSIZE_Y > 1
  UVAR_t         ymask;
#endif
} TBITS_t;


#if SYS_FEATURE_EVENTS != 0

typedef union EVENT {
  struct {
#if POSCFG_ARGCHECK > 1
    UVAR_t       magic;
#endif
    union EVENT *next;
  } l;
  struct {
#if POSCFG_ARGCHECK > 1
    UVAR_t       magic;
#endif
    union {
      INT_t      counter;
#if POSCFG_FEATURE_FLAGS != 0
      UVAR_t     flags;
#endif
    } d;
    TBITS_t      pend;
#if POSCFG_FEATURE_MUTEXES != 0
    POSTASK_t    task;
#endif
  } e;
} *EVENT_t;


static EVENT_t   posFreeEvents_g;

#if (POSCFG_DYNAMIC_MEMORY == 0) && \
    ((POSCFG_MAX_EVENTS + SYS_MSGBOXEVENTS) != 0)
STATICBUFFER(posStaticEventMem_g, sizeof(union EVENT), \
             (POSCFG_MAX_EVENTS + SYS_MSGBOXEVENTS));
#endif

#endif  /* SYS_FEATURE_EVENTS */


#if POSCFG_FEATURE_MSGBOXES != 0

typedef struct MSGBUF {
#if POSCFG_MSG_MEMORY != 0
  unsigned char  buffer[POSCFG_MSG_BUFSIZE];
#else
  void           *bufptr;
#endif
#if POSCFG_ARGCHECK > 1
  UVAR_t         magic;
#endif
  struct MSGBUF *next;
} MSGBUF_t;

static POSSEMA_t msgAllocSyncSem_g;
static POSSEMA_t msgAllocWaitSem_g;
static UVAR_t    msgAllocWaitReq_g;
static MSGBUF_t  *posFreeMessagebuf_g;

#if (POSCFG_DYNAMIC_MEMORY == 0) && (POSCFG_MAX_MESSAGES != 0)
STATICBUFFER(posStaticMessageMem_g, sizeof(MSGBUF_t), POSCFG_MAX_MESSAGES);
#endif

#endif /* POSCFG_FEATURE_MSGBOXES */


#if POSCFG_FEATURE_TIMER != 0

typedef struct TIMER {
#if POSCFG_ARGCHECK > 1
  UVAR_t         magic;
#endif
  struct TIMER   *prev;
  struct TIMER   *next;
  POSSEMA_t      sema;
  UINT_t         counter;
  UINT_t         wait;
  UINT_t         reload;
#if POSCFG_FEATURE_TIMERFIRED != 0
  VAR_t          fired;
#endif
} TIMER_t;

static TIMER_t   *posFreeTimer_g;
static TIMER_t   *posActiveTimers_g;

#if (POSCFG_DYNAMIC_MEMORY == 0) && (POSCFG_MAX_TIMER != 0)
STATICBUFFER(posStaticTmrMem_g, sizeof(TIMER_t), POSCFG_MAX_TIMER);
#endif

#endif /* POSCFG_FEATURE_TIMER */


#if POSCFG_FEATURE_JIFFIES != 0
#if POSCFG_FEATURE_LARGEJIFFIES == 0
JIF_t  jiffies;
#else
static JIF_t     pos_jiffies_g;
#endif
#endif /* POSCFG_FEATURE_JIFFIES */


static UVAR_t    posMustSchedule_g;
static TBITS_t   posReadyTasks_g;
static TBITS_t   posAllocatedTasks_g;
static POSTASK_t posSleepingTasks_g;
static POSTASK_t posFreeTasks_g;
static POSTASK_t posTaskTable_g[SYS_TASKTABSIZE_X * SYS_TASKTABSIZE_Y];

#if POSCFG_CTXSW_COMBINE > 1
static UVAR_t    posCtxCombineCtr_g;
#endif

#if POSCFG_FEATURE_INHIBITSCHED != 0
static UVAR_t    posInhibitSched_g;
#endif

#if POSCFG_FEATURE_IDLETASKHOOK != 0
static POSIDLEFUNC_t  posIdleTaskFuncHook_g;
#endif

#if (POSCFG_DYNAMIC_MEMORY == 0) && (POSCFG_MAX_TASKS != 0)
STATICBUFFER(posStaticTaskMem_g, sizeof(struct POSTASK), POSCFG_MAX_TASKS);
#endif


#if POSCFG_FEATURE_SOFTINTS != 0

static struct {
  UVAR_t         intno;
  UVAR_t         param;
} softintqueue_g[POSCFG_SOFTINTQUEUELEN + 1];
static POSINTFUNC_t softIntHandlers_g[POSCFG_SOFTINTERRUPTS];
static UVAR_t    sintIdxIn_g;
static UVAR_t    sintIdxOut_g;

#endif /* POSCFG_FEATURE_SOFTINTS */


#if MVAR_BITS == 8
UVAR_t posShift1lTab_g[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
#endif



/*-------------------------------------------------------------------------
 *  ARGUMENT CHECKING (3 different levels possible)
 *-------------------------------------------------------------------------*/

#if MVAR_BITS == 8
#define POSMAGIC_TASK   0x4E
#define POSMAGIC_EVENT  0x53
#define POSMAGIC_MSGBUF 0x7F
#define POSMAGIC_TIMER  0x14
#elif MVAR_BITS == 16
#define POSMAGIC_TASK   0x4E56
#define POSMAGIC_EVENT  0x538E
#define POSMAGIC_MSGBUF 0x7FC4
#define POSMAGIC_TIMER  0x1455
#else
#define POSMAGIC_TASK   0x4E56A3FC
#define POSMAGIC_EVENT  0x538EC75B
#define POSMAGIC_MSGBUF 0x7FC45AA2
#define POSMAGIC_TIMER  0x14552384
#endif

#if POSCFG_ARGCHECK == 0
#define POS_ARGCHECK_RET(ptr, pmagic, magic, ret) \
  do { } while(0)
#define POS_ARGCHECK(ptr, pmagic, magic) \
  do { } while(0)
#elif POSCFG_ARGCHECK == 1
#define POS_ARGCHECK_RET(ptr, pmagic, magic, ret) \
  if ((ptr) == NULL) return (ret)
#define POS_ARGCHECK(ptr, pmagic, magic) \
  if ((ptr) == NULL) return
#else
#define POS_ARGCHECK_RET(ptr, pmagic, magic, ret) \
  if (((ptr) == NULL) || (pmagic != magic)) return (ret)
#define POS_ARGCHECK(ptr, pmagic, magic) \
  if (((ptr) == NULL) || (pmagic != magic)) return
#endif



/*---------------------------------------------------------------------------
 * ROUND ROBIN
 *-------------------------------------------------------------------------*/

#if POSCFG_ROUNDROBIN != 0

static UVAR_t  posNextRoundRobin_g[SYS_TASKTABSIZE_Y];
#define POS_NEXTROUNDROBIN(idx)  posNextRoundRobin_g[idx]

#else  /* ROUNDROBIN */

#define POS_NEXTROUNDROBIN(idx)  0

#endif /* ROUNDROBIN */



/*---------------------------------------------------------------------------
 * PROTOTYPES OF PRIVATE FUNCTIONS
 *-------------------------------------------------------------------------*/

static void      pos_schedule(void);
static void      pos_idletask(void *arg);
#if SYS_FEATURE_EVENTS != 0
static VAR_t     pos_sched_event(EVENT_t ev);
#endif
#if (POSCFG_FEATURE_MSGBOXES != 0) && (POSCFG_MSG_MEMORY == 0)
static MSGBUF_t* pos_msgAlloc(void);
static void      pos_msgFree(MSGBUF_t *mbuf);
#endif
#if POSCFG_FEATURE_SOFTINTS != 0
static void      pos_execSoftIntQueue(void);
#endif
#if POSCFG_FEATURE_LISTS != 0
#if POSCFG_FEATURE_LISTJOIN != 0
static void      pos_listJoin(POSLIST_t *prev, POSLIST_t *next,
                              POSLIST_t *newlist);
#endif
static void      pos_listRemove(POSLIST_t *listelem);
#endif



/*---------------------------------------------------------------------------
 * MACROS
 *-------------------------------------------------------------------------*/

#if (POSCFG_FEATURE_YIELD != 0) && \
    (POSCFG_ROUNDROBIN != 0) && (SYS_TASKTABSIZE_Y != 1)
#if (MVAR_BITS == 8)
static UVAR_t posZeroMask_g[7] = {0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};
#define pos_zmask(x)   posZeroMask_g[(UVAR_t)(x)]
#else
#define pos_zmask(x)   (~((pos_shift1l((x) + 1)) - 1))
#endif
#endif


#if SYS_TASKTABSIZE_Y > 1

#define pos_setTableBit(table, task) do { \
    (table)->ymask |= (task)->bit_y; \
    (table)->xtable[(task)->idx_y] |= (task)->bit_x; } while(0)

#define pos_delTableBit(table, task) do { \
    UVAR_t tbt; \
    tbt  = (table)->xtable[(task)->idx_y] & ~(task)->bit_x; \
    (table)->xtable[(task)->idx_y] = tbt; \
    if (tbt == 0) (table)->ymask &= ~(task)->bit_y; } while(0)

#define pos_isTableBitSet(table, task) \
    (((table)->xtable[(task)->idx_y] & (task)->bit_x) != 0)

#else

#define pos_setTableBit(table, task) do { \
    (table)->xtable[0] |= (task)->bit_x; } while(0)

#define pos_delTableBit(table, task) do { \
    (table)->xtable[0] &= ~(task)->bit_x;} while(0)

#define pos_isTableBitSet(table, task) \
    (((table)->xtable[0] & (task)->bit_x) != 0)

#endif

#define pos_addToList(list, elem) do { \
    (elem)->prev = NULL; \
    (elem)->next = list; \
    if (list != NULL) list->prev = elem; \
    list = elem; } while(0)

#define pos_removeFromList(list, elem) do { \
    if ((elem)->next != NULL) (elem)->next->prev = (elem)->prev; \
    if ((elem)->prev != NULL) (elem)->prev->next = (elem)->next; \
    else list = (elem)->next; } while(0)

#if POSCFG_FEATURE_SOFTINTS != 0
#define softIntsPending()  (sintIdxIn_g != sintIdxOut_g)
#define pos_doSoftInts() \
  if (softIntsPending())  pos_execSoftIntQueue();
#else
#define pos_doSoftInts() do { } while(0);
#endif /* POSCFG_FEATURE_SOFTINTS */

#ifndef HOOK_IDLETASK
#define HOOK_IDLETASK
#endif



/*---------------------------------------------------------------------------
 * some HELPER FUNCTIONS  (can be inlined)
 *-------------------------------------------------------------------------*/

#if SYS_TASKDOUBLELINK == 0
#define pos_addToSleepList(task) do { \
    (task)->next = posSleepingTasks_g; \
    posSleepingTasks_g = task; } while(0)
#endif


#if SYS_FEATURE_EVENTS != 0
#if (POSCFG_FASTCODE == 0) && (SYS_EVENTS_USED != 0)

static void pos_eventAddTask(EVENT_t ev, POSTASK_t task);
static void pos_eventAddTask(EVENT_t ev, POSTASK_t task)
{
  pos_setTableBit(&ev->e.pend, task);
}

static void pos_eventRemoveTask(EVENT_t ev, POSTASK_t task);
static void pos_eventRemoveTask(EVENT_t ev, POSTASK_t task)
{
  pos_delTableBit(&ev->e.pend, task);
}

#else
#define pos_eventAddTask(event, curtask) \
          pos_setTableBit(&((event)->e.pend), curtask)
#define pos_eventRemoveTask(event, curtask) \
          pos_delTableBit(&((event)->e.pend), curtask)
#endif
#endif  /* SYS_FEATURE_EVENTS */


#if POSCFG_FASTCODE != 0

#define pos_enableTask(task)    pos_setTableBit(&posReadyTasks_g, task)
#define pos_disableTask(task)   pos_delTableBit(&posReadyTasks_g, task)

#if SYS_TASKDOUBLELINK != 0
#define pos_addToSleepList(task) \
          pos_addToList(posSleepingTasks_g, task)
#define pos_removeFromSleepList(task) \
          pos_removeFromList(posSleepingTasks_g, task)
#endif  /* SYS_TASKDOUBLELINK */

#if POSCFG_FEATURE_TIMER != 0
#define pos_addToTimerList(timer) \
          pos_addToList(posActiveTimers_g, timer)
#define pos_removeFromTimerList(timer) do { \
          pos_removeFromList(posActiveTimers_g, timer); \
          timer->prev = timer; } while (0)
#endif  /* POSCFG_FEATURE_TIMER */

#else /* POSCFG_FASTCODE */

static void pos_disableTask(POSTASK_t task);
static void pos_disableTask(POSTASK_t task)
{
  pos_delTableBit(&posReadyTasks_g, task);
}

static void pos_enableTask(POSTASK_t task);
static void pos_enableTask(POSTASK_t task)
{
  pos_setTableBit(&posReadyTasks_g, task);
}

#if SYS_TASKDOUBLELINK != 0
static void pos_addToSleepList(POSTASK_t task);
static void pos_addToSleepList(POSTASK_t task)
{
  pos_addToList(posSleepingTasks_g, task);
}

static void pos_removeFromSleepList(POSTASK_t task);
static void pos_removeFromSleepList(POSTASK_t task)
{
  pos_removeFromList(posSleepingTasks_g, task);
}
#endif  /* SYS_TASKDOUBLELINK */

#if POSCFG_FEATURE_TIMER != 0
static void pos_addToTimerList(TIMER_t *timer);
static void pos_addToTimerList(TIMER_t *timer)
{
  pos_addToList(posActiveTimers_g, timer);
}

static void pos_removeFromTimerList(TIMER_t *timer);
static void pos_removeFromTimerList(TIMER_t *timer)
{
  pos_removeFromList(posActiveTimers_g, timer);
  timer->prev = timer;
}
#endif  /* POSCFG_FEATURE_TIMER */

#endif  /* POSCFG_FASTCODE */



/*---------------------------------------------------------------------------
 * PRIVATE FUNCTIONS
 *-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_LISTS != 0
#if POSCFG_FEATURE_LISTJOIN != 0
static void pos_listJoin(POSLIST_t *prev, POSLIST_t *next,
                         POSLIST_t *newlist)
{
  next->prev          = newlist->prev;
  newlist->prev->next = next;
  newlist->prev       = prev;
  prev->next          = newlist;
}
#endif

static void pos_listRemove(POSLIST_t *listelem)
{
  register POSLIST_t *prev, *next;
  prev = listelem->prev;
  next = listelem->next;  
  next->prev = prev;
  prev->next = next;
}
#endif /* POSCFG_FEATURE_LISTS */

static void pos_idletask(void *arg)
{
  POS_LOCKFLAGS;

  (void) arg;

  for(;;)
  {
    POS_SCHED_LOCK;
    pos_schedule();
    POS_SCHED_UNLOCK;
    HOOK_IDLETASK
#if POSCFG_FEATURE_IDLETASKHOOK != 0
    if (posIdleTaskFuncHook_g != NULL)
      (posIdleTaskFuncHook_g)();
#endif
  }
}

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_SOFTINTS != 0

static void pos_execSoftIntQueue(void)
{
  register UVAR_t intno;

  ++posInInterrupt_g;
  do
  {
    intno = softintqueue_g[sintIdxOut_g].intno;
    if (softIntHandlers_g[intno] != NULL)
    {
      (softIntHandlers_g[intno])(softintqueue_g[sintIdxOut_g].param);
    }
    if (++sintIdxOut_g > POSCFG_SOFTINTQUEUELEN)
      sintIdxOut_g = 0;
  }
  while (sintIdxIn_g != sintIdxOut_g);
  --posInInterrupt_g;
}

#endif /* POSCFG_FEATURE_SOFTINTS */

/*-------------------------------------------------------------------------*/

static void pos_schedule(void)
{
  register UVAR_t ym, xt;

  if (posInInterrupt_g == 0)
  {
    pos_doSoftInts();
#if POSCFG_FEATURE_INHIBITSCHED != 0
    if (posInhibitSched_g == 0)
    {
#endif
      posMustSchedule_g = 0;
#if POSCFG_CTXSW_COMBINE > 1
      posCtxCombineCtr_g = 0;
#endif

#if SYS_TASKTABSIZE_Y > 1
      ym = POS_FINDBIT(posReadyTasks_g.ymask);
#else
      ym = 0;
#endif
      xt = POS_FINDBIT_EX(posReadyTasks_g.xtable[ym],
                          POS_NEXTROUNDROBIN(ym));

#if (SYS_TASKTABSIZE_X > 1) && (POSCFG_ROUNDROBIN != 0)
      posNextRoundRobin_g[ym] = (xt + 1) & (SYS_TASKTABSIZE_X - 1);
#endif

      posNextTask_g = posTaskTable_g[(ym * SYS_TASKTABSIZE_X) + xt];

      if (posCurrentTask_g != posNextTask_g)
      {
        p_pos_softContextSwitch();
      }
#if POSCFG_FEATURE_INHIBITSCHED != 0
    }
#endif
  }
}

/*-------------------------------------------------------------------------*/

#if SYS_FEATURE_EVENTS != 0

static VAR_t pos_sched_event(EVENT_t ev)
{
  register UVAR_t  ym, xt;
  register POSTASK_t task;

#if POSCFG_FEATURE_SOFTINTS != 0
  if (softIntsPending())
  {
    pos_schedule();
  }
#endif

#if SYS_TASKTABSIZE_Y > 1
  if (ev->e.pend.ymask != 0)
  {
    ym = POS_FINDBIT(ev->e.pend.ymask);
    xt = POS_FINDBIT_EX(ev->e.pend.xtable[ym],
                        POS_NEXTROUNDROBIN(ym));
#else
  if (ev->e.pend.xtable[0] != 0)
  {
    ym = 0;
    xt = POS_FINDBIT(ev->e.pend.xtable[0]);
#endif

    task = posTaskTable_g[(ym * SYS_TASKTABSIZE_X) + xt];

    pos_eventRemoveTask(ev, task);
    pos_enableTask(task);
    posMustSchedule_g = 1;

#if (POSCFG_SOFT_MTASK == 0) || (POSCFG_CTXSW_COMBINE == 1)
    pos_schedule();
#else
#if POSCFG_REALTIME_PRIO > 0
#if (POSCFG_ROUNDROBIN != 0) && (SYS_TASKTABSIZE_Y > 1)
    if (task->idx_y  <  (SYS_TASKTABSIZE_Y - POSCFG_REALTIME_PRIO))
    {
      pos_schedule();
    }
    else
#endif
#endif
#if POSCFG_CTXSW_COMBINE > 1
    if (++posCtxCombineCtr_g >= POSCFG_CTXSW_COMBINE)
    {
      pos_schedule();
    }
    else
#endif
    do { } while(0);
#endif /* else (POSCFG_SOFT_MTASK == 0) || (POSCFG_CTXSW_COMBINE == 1) */
    return 1;
  }
  return 0;
}

#endif  /* SYS_FEATURE_EVENTS */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  ERROR NUMBER VARIABLE
 *-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_ERRNO != 0

VAR_t* _errno_p(void)
{
  return &posCurrentTask_g->error;
}

#endif  /* POSCFG_FEATURE_ERRNO */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  INTERRUPT CONTROL
 *-------------------------------------------------------------------------*/

void c_pos_intEnter(void)
{
#if POSCFG_ISR_INTERRUPTABLE != 0
  POS_LOCKFLAGS;
  POS_SCHED_LOCK;
  ++posInInterrupt_g;
  POS_SCHED_UNLOCK;
#else
  ++posInInterrupt_g;
#endif
}

/*-------------------------------------------------------------------------*/

void c_pos_intExit(void)
{
  register UVAR_t ym, xt;
  POS_LOCKFLAGS;

#if POSCFG_ISR_INTERRUPTABLE != 0
  POS_SCHED_LOCK;
#endif

  if (--posInInterrupt_g == 0)
  {
    pos_doSoftInts();
#if POSCFG_FEATURE_INHIBITSCHED != 0
    if (posInhibitSched_g == 0)
    {
#endif
      if (posMustSchedule_g != 0)
      {
        posMustSchedule_g = 0;
#if POSCFG_CTXSW_COMBINE > 1
        posCtxCombineCtr_g = 0;
#endif

#if SYS_TASKTABSIZE_Y > 1
        ym = POS_FINDBIT(posReadyTasks_g.ymask);
#else
        ym = 0;
#endif
        xt = POS_FINDBIT_EX(posReadyTasks_g.xtable[ym],
                            POS_NEXTROUNDROBIN(ym));

#if (SYS_TASKTABSIZE_X > 1) && (POSCFG_ROUNDROBIN != 0)
        posNextRoundRobin_g[ym] = (xt + 1) & (SYS_TASKTABSIZE_X - 1);
#endif

        posNextTask_g = posTaskTable_g[(ym * SYS_TASKTABSIZE_X) + xt];

        if (posCurrentTask_g != posNextTask_g)
        {
#if POSCFG_ISR_INTERRUPTABLE == 0
          /* all ctx switch functions need to be called with lock acquired */
          POS_SCHED_LOCK;
#endif
          /* Note:
           * The processor does not return from this function call. When
           * this function returns anyway, the architecture port is buggy.
           */
          p_pos_intContextSwitch();
        }
      }
#if POSCFG_FEATURE_INHIBITSCHED != 0
    }
#endif
  }
#if POSCFG_ISR_INTERRUPTABLE != 0
  POS_SCHED_UNLOCK;
#endif
}

/*-------------------------------------------------------------------------*/

void c_pos_timerInterrupt(void)
{
  register POSTASK_t  task;
#if SYS_TASKDOUBLELINK == 0
  register POSTASK_t  last = NULL;
#endif
#if POSCFG_FEATURE_TIMER != 0
  register TIMER_t   *tmr;
#endif
#if POSCFG_ISR_INTERRUPTABLE != 0
  POS_LOCKFLAGS;
#endif

  if (posRunning_g == 0)
    return;

#if POSCFG_ISR_INTERRUPTABLE != 0
  POS_SCHED_LOCK;
#endif

#if POSCFG_FEATURE_JIFFIES != 0
#if POSCFG_FEATURE_LARGEJIFFIES == 0
  ++jiffies;
#else
  ++pos_jiffies_g;
#endif
#endif

#if POSCFG_FEATURE_TIMER != 0
  tmr = posActiveTimers_g;
  while (tmr != NULL)
  {
    --(tmr->counter);
    if (tmr->counter == 0)
    {
      posSemaSignal(tmr->sema);
#if POSCFG_FEATURE_TIMERFIRED != 0
      tmr->fired = 1;
#endif
      if (tmr->reload != 0)
      {
        tmr->counter = tmr->reload;
      }
      else
      {
        pos_removeFromTimerList(tmr);
      }
    }
    tmr = tmr->next;
  }
#endif

  task = posSleepingTasks_g;
  while (task != NULL)
  {
    --(task->ticks);
    if (task->ticks == 0)
    {
      pos_enableTask(task);
#if SYS_TASKDOUBLELINK != 0
      pos_removeFromSleepList(task);
      task->prev = task;
    }
    task = task->next;
#else
      task = task->next;
      if (last == NULL)
      {
        posSleepingTasks_g = task;
      }
      else
      {
        last->next = task;
      }
    }
    else
    {
      last = task;
      task = task->next;
    }
#endif
  }
  posMustSchedule_g = 1;
#if POSCFG_ISR_INTERRUPTABLE != 0
  POS_SCHED_UNLOCK;
#endif
}



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  TASK CONTROL
 *-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_YIELD != 0

void posTaskYield(void)
{
#if (POSCFG_ROUNDROBIN == 0) || (SYS_TASKTABSIZE_Y == 1)
  POS_LOCKFLAGS;
  POS_SCHED_LOCK;
  pos_schedule();
  POS_SCHED_UNLOCK;
#else

  register UVAR_t p, ym, xt;
  POS_LOCKFLAGS;

  POS_SCHED_LOCK;
  if (posInInterrupt_g == 0)
  {
    pos_doSoftInts();
#if POSCFG_FEATURE_INHIBITSCHED != 0
    if (posInhibitSched_g == 0)
    {
#endif
      p = posCurrentTask_g->idx_y;
      if ((p >= (SYS_TASKTABSIZE_Y - 1)) ||
          (posMustSchedule_g != 0))
      {
        pos_schedule();
        POS_SCHED_UNLOCK;
        return;
      }

#if POSCFG_CTXSW_COMBINE > 1
      posCtxCombineCtr_g = 0;
#endif

      ym = POS_FINDBIT(posReadyTasks_g.ymask);
      if (ym == p)
      {
        if ((UVAR_t)(posReadyTasks_g.xtable[ym] &
            ~posCurrentTask_g->bit_x) == 0)
        {
          ym = POS_FINDBIT(posReadyTasks_g.ymask & pos_zmask(ym));
        }
      }

      xt = POS_FINDBIT_EX(posReadyTasks_g.xtable[ym],
                          POS_NEXTROUNDROBIN(ym));

#if SYS_TASKTABSIZE_X > 1
      posNextRoundRobin_g[ym] = (xt + 1) & (SYS_TASKTABSIZE_X - 1);
#endif

      posNextTask_g = posTaskTable_g[(ym * SYS_TASKTABSIZE_X) + xt];

      if (posCurrentTask_g != posNextTask_g)
      {
        p_pos_softContextSwitch();
      }
#if POSCFG_FEATURE_INHIBITSCHED != 0
    }
#endif
  }
  POS_SCHED_UNLOCK;
#endif
}

#endif  /* POSCFG_FEATURE_YIELD */

/*-------------------------------------------------------------------------*/

#if POSCFG_TASKSTACKTYPE == 0
POSTASK_t posTaskCreate(POSTASKFUNC_t funcptr, void *funcarg,
                        VAR_t priority, void *stackstart)
#elif POSCFG_TASKSTACKTYPE == 1
POSTASK_t posTaskCreate(POSTASKFUNC_t funcptr, void *funcarg,
                        VAR_t priority, UINT_t stacksize)
#else
POSTASK_t posTaskCreate(POSTASKFUNC_t funcptr, void *funcarg,
                        VAR_t priority)
#endif
{
  register POSTASK_t  task;
  register UVAR_t  b, p;
  register unsigned char *m;
  register UINT_t i;
  POS_LOCKFLAGS;

  if ((UVAR_t)priority >= POSCFG_MAX_PRIO_LEVEL)
    return NULL;

  POS_SCHED_LOCK;
  task = posFreeTasks_g;
#if SYS_POSTALLOCATE != 0
  if (task == NULL)
  {
    POS_SCHED_UNLOCK;
    task = (POSTASK_t) POS_MEM_ALLOC(sizeof(struct POSTASK) +
                                     (POSCFG_ALIGNMENT - 1));
    if (task == NULL)
      return NULL;

    task = MEMALIGN(POSTASK_t, task);
    POS_SCHED_LOCK;
    task->next = posFreeTasks_g;
    posFreeTasks_g = task;
  }
#endif /* SYS_POSTALLOCATE */

#if POSCFG_ROUNDROBIN == 0
  p = (SYS_TASKTABSIZE_Y - 1) - (priority / MVAR_BITS);
  b = (~posAllocatedTasks_g.xtable[p]) &
      pos_shift1l((MVAR_BITS-1) - (priority & (MVAR_BITS-1)));
#else
  p = (SYS_TASKTABSIZE_Y - 1) - priority;
  b = ~posAllocatedTasks_g.xtable[p];
#endif

  if ((b == 0) || (task == NULL))
  {
    POS_SCHED_UNLOCK;
    return NULL;
  }

  b = POS_FINDBIT(b);
#if (POSCFG_ROUNDROBIN != 0) && (SYS_TASKTABSIZE_X < MVAR_BITS)
  if (b >= SYS_TASKTABSIZE_X)
  {
    POS_SCHED_UNLOCK;
    return NULL;
  }
#endif
  posFreeTasks_g = task->next;

  m = (unsigned char*) task;
  i = sizeof(struct POSTASK);
  while (i != 0) { *m = 0; ++m; --i; };

#if POSCFG_ARGCHECK > 1
  task->magic = POSMAGIC_TASK;
#endif
#if SYS_TASKTABSIZE_Y > 1
  task->idx_y = p;
  task->bit_y = pos_shift1l(p);
#endif
  task->bit_x = pos_shift1l(b);
  posTaskTable_g[(p * SYS_TASKTABSIZE_X) + b] = task;

#if POSCFG_TASKSTACKTYPE == 0
  p_pos_initTask(task, stackstart, funcptr, funcarg);
#elif POSCFG_TASKSTACKTYPE == 1
  if (p_pos_initTask(task, stacksize, funcptr, funcarg) != 0)
  {
    task->next = posFreeTasks_g;
    posFreeTasks_g = task;
    POS_SCHED_UNLOCK;
    return NULL;
  }
#else
  if (p_pos_initTask(task, funcptr, funcarg) != 0)
  {
    task->next = posFreeTasks_g;
    posFreeTasks_g = task;
    POS_SCHED_UNLOCK;
    return NULL;
  }
#endif

#if SYS_TASKSTATE != 0
  task->state = POSTASKSTATE_ACTIVE;
#endif
  pos_setTableBit(&posAllocatedTasks_g, task);
  pos_enableTask(task);
  pos_schedule();
  POS_SCHED_UNLOCK;
  return task;
}

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_EXIT != 0

void posTaskExit(void)
{
  register POSTASK_t task = posCurrentTask_g;
  POS_LOCKFLAGS;

#if POSCFG_FEATURE_MSGBOXES != 0
  if (task->msgsem != NULL)
  {
    posSemaDestroy(task->msgsem);
  }
  POS_SCHED_LOCK;
  task->state = POSTASKSTATE_ZOMBIE;
  if (task->firstmsg != NULL)
  {
    ((MSGBUF_t*)(task->lastmsg))->next = posFreeMessagebuf_g;
    posFreeMessagebuf_g = (MSGBUF_t*)(task->firstmsg);
    if (msgAllocWaitReq_g != 0)
    {
      msgAllocWaitReq_g = 0;
      POS_SCHED_UNLOCK;
      posSemaSignal(msgAllocWaitSem_g);
      POS_SCHED_LOCK;
    }
  }
#else
  POS_SCHED_LOCK;
#endif
  pos_disableTask(task);
  pos_delTableBit(&posAllocatedTasks_g, task);
#if (POSCFG_TASKSTACKTYPE == 1) || (POSCFG_TASKSTACKTYPE == 2)
  p_pos_freeStack(task);
#endif
#if POSCFG_STKFREE_HOOK != 0
  if (task->stkfree != NULL)
    (task->stkfree)(task);
#endif
#if SYS_TASKSTATE != 0
  task->state = POSTASKSTATE_UNUSED;
#endif
  task->next = posFreeTasks_g;
  posFreeTasks_g = task;
  pos_schedule();
  for(;;);
}

#endif  /* POSCFG_FEATURE_EXIT */

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_GETTASK != 0

POSTASK_t posTaskGetCurrent(void)
{
  return posCurrentTask_g;
}

#endif  /* POSCFG_FEATURE_GETTASK */

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_TASKUNUSED != 0

VAR_t posTaskUnused(POSTASK_t taskhandle)
{
  POS_ARGCHECK_RET(taskhandle, taskhandle->magic, POSMAGIC_TASK, -E_ARG); 
  return (taskhandle->state == POSTASKSTATE_UNUSED) ? 1 : 0;
}

#endif  /* POSCFG_FEATURE_TASKUNUSED */

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_SETPRIORITY != 0

VAR_t posTaskSetPriority(POSTASK_t taskhandle, VAR_t priority)
{
  register UVAR_t  b, p;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(taskhandle, taskhandle->magic, POSMAGIC_TASK, -E_ARG); 
  if ((UVAR_t)priority >= POSCFG_MAX_PRIO_LEVEL)
    return -E_ARG;

  POS_SCHED_LOCK;
#if POSCFG_ROUNDROBIN == 0
  p = (SYS_TASKTABSIZE_Y - 1) - (priority / MVAR_BITS);
  b = (~posAllocatedTasks_g.xtable[p]) &
      pos_shift1l((MVAR_BITS-1) - (priority & (MVAR_BITS-1)));
#else
  p = (SYS_TASKTABSIZE_Y - 1) - priority;
  b = ~posAllocatedTasks_g.xtable[p];
#endif
  if (b == 0)
  {
    POS_SCHED_UNLOCK;
    return -E_FAIL;
  }
  b = POS_FINDBIT(b);
#if (POSCFG_ROUNDROBIN != 0) && (SYS_TASKTABSIZE_X < MVAR_BITS)
  if (b >= SYS_TASKTABSIZE_X)
  {
    POS_SCHED_UNLOCK;
    return -E_FAIL;
  }
#endif
  pos_disableTask(taskhandle);
  pos_delTableBit(&posAllocatedTasks_g, taskhandle);
#if SYS_TASKTABSIZE_Y > 1
  taskhandle->idx_y = p;
  taskhandle->bit_y = pos_shift1l(p);
#endif
  taskhandle->bit_x = pos_shift1l(b);
  posTaskTable_g[(p * SYS_TASKTABSIZE_X) + b] = taskhandle;
  pos_setTableBit(&posAllocatedTasks_g, taskhandle);
  pos_enableTask(taskhandle);
  POS_SCHED_UNLOCK;
  return E_OK;
}

#endif  /* POSCFG_FEATURE_SETPRIORITY */

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_GETPRIORITY != 0

VAR_t posTaskGetPriority(POSTASK_t taskhandle)
{
  register VAR_t p;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(taskhandle, taskhandle->magic, POSMAGIC_TASK, -E_ARG); 
  POS_SCHED_LOCK;
#if SYS_TASKTABSIZE_Y == 1
  p = 0;
#else
  p = (SYS_TASKTABSIZE_Y - 1) - taskhandle->idx_y;
#endif
#if POSCFG_ROUNDROBIN == 0
  p = (p * MVAR_BITS) + (MVAR_BITS - 1) - POS_FINDBIT(taskhandle->bit_x);
#endif
  POS_SCHED_UNLOCK;
  return p;
}

#endif  /* POSCFG_FEATURE_GETPRIORITY */

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_SLEEP != 0

void posTaskSleep(UINT_t ticks)
{
  register POSTASK_t task;
  POS_LOCKFLAGS;

#if POSCFG_ARGCHECK > 1
  if (posInInterrupt_g != 0)
    return;
#endif

  POS_SCHED_LOCK;
  if (ticks != 0)
  {
    task = posCurrentTask_g;
    task->ticks = ticks;
    pos_disableTask(task);
    pos_addToSleepList(task);
  }
  pos_schedule();
  POS_SCHED_UNLOCK;
}

#endif  /* POSCFG_FEATURE_SLEEP */

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_INHIBITSCHED != 0

void posTaskSchedLock(void)
{
  POS_LOCKFLAGS;

  POS_SCHED_LOCK;
  ++posInhibitSched_g;
  POS_SCHED_UNLOCK;
}

/*-------------------------------------------------------------------------*/

void posTaskSchedUnlock(void)
{
  POS_LOCKFLAGS;

  POS_SCHED_LOCK;
  --posInhibitSched_g;
  if ((posInhibitSched_g == 0) &&
      (posMustSchedule_g != 0))
  {
    pos_schedule();
  }
  POS_SCHED_UNLOCK;
}

#endif  /* POSCFG_FEATURE_INHIBITSCHED */

/*-------------------------------------------------------------------------*/

#if POSCFG_TASKCB_USERSPACE > 0

void*  posTaskGetUserspace(void)
{
  return (void*) &posCurrentTask_g->usrspace[0];
}

#endif  /* POSCFG_TASKCB_USERSPACE */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  SEMAPHORES
 *-------------------------------------------------------------------------*/

#if SYS_FEATURE_EVENTS != 0

POSSEMA_t posSemaCreate(INT_t initcount)
{
  register EVENT_t ev;
  register UVAR_t i;
  POS_LOCKFLAGS;

  POS_SCHED_LOCK;
  ev = posFreeEvents_g;

#if SYS_POSTALLOCATE != 0
  if (ev == NULL)
  {
    POS_SCHED_UNLOCK;
    ev = (EVENT_t) POS_MEM_ALLOC(sizeof(union EVENT) +
                                 (POSCFG_ALIGNMENT - 1));
    if (ev == NULL)
      return NULL;

    ev = MEMALIGN(EVENT_t, ev);
#if POSCFG_ARGCHECK > 1
    ev->e.magic = POSMAGIC_EVENT;
#endif
    POS_SCHED_LOCK;
  }
  else
  {
#if POSCFG_ARGCHECK > 1
    if (ev->e.magic != POSMAGIC_EVENT)
    {
      POS_SCHED_UNLOCK;
      return NULL;
    }
#endif
    posFreeEvents_g = ev->l.next;
  }
#else  /* SYS_POSTALLOCATE */

  if (ev != NULL)
  {
#if POSCFG_ARGCHECK > 1
    if (ev->e.magic != POSMAGIC_EVENT)
    {
      POS_SCHED_UNLOCK;
      return NULL;
    }
#endif
    posFreeEvents_g = ev->l.next;
#endif /* SYS_POSTALLOCATE */

    ev->e.d.counter = initcount;
#if POSCFG_FEATURE_MUTEXES != 0
    ev->e.task = NULL;
#endif
    for (i=0; i<SYS_TASKTABSIZE_Y; ++i)
    {
      ev->e.pend.xtable[i] = 0;
    }
#if SYS_TASKTABSIZE_Y > 1
    ev->e.pend.ymask = 0;
#endif
#if SYS_POSTALLOCATE == 0
  }
#endif
  POS_SCHED_UNLOCK;
  return (POSSEMA_t) ev;
}

/*-------------------------------------------------------------------------*/

#if SYS_FEATURE_EVENTFREE != 0

void posSemaDestroy(POSSEMA_t sema)
{
  register EVENT_t ev = (EVENT_t) sema;
  POS_LOCKFLAGS;

  POS_ARGCHECK(ev, ev->e.magic, POSMAGIC_EVENT); 
#if SYS_TASKTABSIZE_Y > 1
  if (ev->e.pend.ymask == 0)
#else
  if (ev->e.pend.xtable[0] == 0)
#endif
  {
    POS_SCHED_LOCK;
    ev->l.next = posFreeEvents_g;
    posFreeEvents_g = ev;
    POS_SCHED_UNLOCK;
  }
}

#endif  /* SYS_FEATURE_EVENTFREE */

/*-------------------------------------------------------------------------*/

#if (POSCFG_SMALLCODE == 0) || (POSCFG_FEATURE_SEMAWAIT == 0)
VAR_t posSemaGet(POSSEMA_t sema)
{
  register EVENT_t  ev = (EVENT_t) sema;
  register POSTASK_t task = posCurrentTask_g;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(ev, ev->e.magic, POSMAGIC_EVENT, -E_ARG); 
#if POSCFG_ARGCHECK > 1
  if (posInInterrupt_g != 0)
    return -E_FORB;
#endif
  POS_SCHED_LOCK;
  if (ev->e.d.counter > 0)
  {
    --(ev->e.d.counter);
  }
  else
  {
    pos_disableTask(task);
    pos_eventAddTask(ev, task);
    pos_schedule();
  }
  POS_SCHED_UNLOCK;
  return E_OK;
}
#endif

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_SEMAWAIT != 0

VAR_t posSemaWait(POSSEMA_t sema, UINT_t timeoutticks)
{
  register EVENT_t  ev = (EVENT_t) sema;
  register POSTASK_t task = posCurrentTask_g;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(ev, ev->e.magic, POSMAGIC_EVENT, -E_ARG); 
#if POSCFG_ARGCHECK > 1
  if (posInInterrupt_g != 0)
    return -E_FORB;
#endif
  POS_SCHED_LOCK;

  if (ev->e.d.counter > 0)
  {
    --(ev->e.d.counter);
  }
  else
  {
    if (timeoutticks != INFINITE)
    {
      if (timeoutticks == 0)
      {
        POS_SCHED_UNLOCK;
        return 1;
      }
      task->ticks = timeoutticks;
      pos_addToSleepList(task);
    }

    pos_disableTask(task);
    pos_eventAddTask(ev, task);
    pos_schedule();

    if (timeoutticks != INFINITE)
    {
      if (task->prev == task)
      {
        if (pos_isTableBitSet(&ev->e.pend, task))
        {
          pos_eventRemoveTask(ev, task);
          POS_SCHED_UNLOCK;
          return 1;
        }
      }
      else
      {
        pos_removeFromSleepList(task);
      }
    }
  }
  POS_SCHED_UNLOCK;
  return E_OK;
}

#endif  /* POSCFG_FEATURE_SEMAWAIT */

/*-------------------------------------------------------------------------*/

VAR_t posSemaSignal(POSSEMA_t sema)
{
  register EVENT_t  ev = (EVENT_t) sema;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(ev, ev->e.magic, POSMAGIC_EVENT, -E_ARG); 
  POS_SCHED_LOCK;

  if (ev->e.d.counter == 0)
  {
    if (pos_sched_event(ev) == 0)
    {
      ev->e.d.counter = 1;
    }
  }
  else
  {
    if (ev->e.d.counter != (((UINT_t)~0) >> 1))
    {
      ++(ev->e.d.counter);
    }
  }
  POS_SCHED_UNLOCK;
  return E_OK;
}

#endif  /* SYS_FEATURE_EVENTS */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  MUTEXES
 *-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_MUTEXES != 0

POSMUTEX_t posMutexCreate(void)
{
  return (POSMUTEX_t) posSemaCreate(1);
}

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_MUTEXDESTROY != 0

void posMutexDestroy(POSMUTEX_t mutex)
{
  posSemaDestroy((POSSEMA_t) mutex);
}

#endif  /* POSCFG_FEATURE_MUTEXDESTROY */

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_MUTEXTRYLOCK != 0

VAR_t posMutexTryLock(POSMUTEX_t mutex)
{
  register EVENT_t  ev = (EVENT_t) mutex;
  register POSTASK_t task = posCurrentTask_g;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(ev, ev->e.magic, POSMAGIC_EVENT, -E_ARG); 
  POS_SCHED_LOCK;

  if (ev->e.task == task)
  {
    --(ev->e.d.counter);
  }
  else
  {
    if (ev->e.d.counter < 1)
    {
      POS_SCHED_UNLOCK;
      return 1;  /* no lock */
    }
    ev->e.d.counter = 0;
    ev->e.task = task;
  }
  POS_SCHED_UNLOCK;
  return 0;  /* have lock */
}

#endif  /* POSCFG_FEATURE_MUTEXTRYLOCK */

/*-------------------------------------------------------------------------*/

VAR_t posMutexLock(POSMUTEX_t mutex)
{
  register EVENT_t  ev = (EVENT_t) mutex;
  register POSTASK_t task = posCurrentTask_g;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(ev, ev->e.magic, POSMAGIC_EVENT, -E_ARG); 
  POS_SCHED_LOCK;

  if (ev->e.task == task)
  {
    --(ev->e.d.counter);
  }
  else
  {
    if (ev->e.d.counter > 0)
    {
      ev->e.d.counter = 0;
    }
    else
    {
      pos_disableTask(task);
      pos_eventAddTask(ev, task);
      pos_schedule();
    }
    ev->e.task = task;
  }
  POS_SCHED_UNLOCK;
  return E_OK;
}

/*-------------------------------------------------------------------------*/

VAR_t posMutexUnlock(POSMUTEX_t mutex)
{
  register EVENT_t  ev = (EVENT_t) mutex;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(ev, ev->e.magic, POSMAGIC_EVENT, -E_ARG); 
  POS_SCHED_LOCK;

  if (ev->e.d.counter == 0)
  {
    ev->e.task = NULL;
    if (pos_sched_event(ev) == 0)
    {
      ev->e.d.counter = 1;
    }
  }
  else
  {
    ++(ev->e.d.counter);
  }
  POS_SCHED_UNLOCK;
  return E_OK;
}

#endif  /* POSCFG_FEATURE_MUTEXES */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  MESSAGE BOXES
 *-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_MSGBOXES != 0

#if POSCFG_MSG_MEMORY == 0
static MSGBUF_t* pos_msgAlloc(void)
#else
void* posMessageAlloc(void)
#endif
{
  register MSGBUF_t *mbuf;
  POS_LOCKFLAGS;

  if ((posInInterrupt_g != 0)
#if POSCFG_FEATURE_INHIBITSCHED != 0
      || (posInhibitSched_g != 0)
#endif
    )
  {
#if POSCFG_ISR_INTERRUPTABLE != 0
    POS_SCHED_LOCK;
#endif
    mbuf = posFreeMessagebuf_g;
    if (mbuf != NULL)
    {
      posFreeMessagebuf_g = (MSGBUF_t*) mbuf->next;
    }
#if POSCFG_ISR_INTERRUPTABLE != 0
    POS_SCHED_UNLOCK;
#endif
#if POSCFG_MSG_MEMORY == 0
    return mbuf;
#else
    return (void*) mbuf;
#endif
  }

#if SYS_POSTALLOCATE != 0
  POS_SCHED_LOCK;
  mbuf = posFreeMessagebuf_g;
  if (mbuf != NULL)
  {
    posFreeMessagebuf_g = (MSGBUF_t*) mbuf->next;
    POS_SCHED_UNLOCK;
#if POSCFG_MSG_MEMORY == 0
    return mbuf;
#else
    return (void*) mbuf;
#endif
  }
  POS_SCHED_UNLOCK;
  mbuf = (MSGBUF_t*) POS_MEM_ALLOC(sizeof(MSGBUF_t) +
                                   (POSCFG_ALIGNMENT - 1));
  if (mbuf != NULL)
  {
    mbuf = MEMALIGN(MSGBUF_t*, mbuf);
#if POSCFG_ARGCHECK > 1
    mbuf->magic = POSMAGIC_MSGBUF;
#endif
#if POSCFG_MSG_MEMORY == 0
    return mbuf;
#else
    return (void*) mbuf;
#endif
  }
#endif /* SYS_POSTALLOCATE */

  posSemaGet(msgAllocSyncSem_g);
  POS_SCHED_LOCK;
  mbuf = posFreeMessagebuf_g;
  while (mbuf == NULL)
  {
    msgAllocWaitReq_g = 1;
    POS_SCHED_UNLOCK;
    posSemaGet(msgAllocWaitSem_g);
    POS_SCHED_LOCK;
    mbuf = posFreeMessagebuf_g;
  }
  posFreeMessagebuf_g = (MSGBUF_t*) mbuf->next;
  POS_SCHED_UNLOCK;
  posSemaSignal(msgAllocSyncSem_g);
#if POSCFG_MSG_MEMORY == 0
  return mbuf;
#else
  return (void*) mbuf;
#endif
}

/*-------------------------------------------------------------------------*/

#if POSCFG_MSG_MEMORY == 0
static void pos_msgFree(MSGBUF_t *mbuf)
{
  POS_LOCKFLAGS;
#else
void posMessageFree(void *buf)
{
  MSGBUF_t *mbuf = (MSGBUF_t*) buf;
  POS_LOCKFLAGS;

  POS_ARGCHECK(mbuf, mbuf->magic, POSMAGIC_MSGBUF); 
#endif
  POS_SCHED_LOCK;
  mbuf->next = (void*) posFreeMessagebuf_g;
  posFreeMessagebuf_g = mbuf;
  if (msgAllocWaitReq_g != 0)
  {
    msgAllocWaitReq_g = 0;
    POS_SCHED_UNLOCK;
    posSemaSignal(msgAllocWaitSem_g);
    return;
  }
  POS_SCHED_UNLOCK;
}

/*-------------------------------------------------------------------------*/

VAR_t posMessageSend(void *buf, POSTASK_t taskhandle)
{
  register MSGBUF_t *mbuf;
  POS_LOCKFLAGS;

#if POSCFG_ARGCHECK != 0
  if ((taskhandle == NULL)
#if POSCFG_ARGCHECK > 1
      || (taskhandle->magic != POSMAGIC_TASK)
#endif
#if (POSCFG_FEATURE_MSGWAIT != 0) && (POSCFG_MSG_MEMORY == 0)
      || (buf == NULL)
#endif
     )
  {
#if POSCFG_MSG_MEMORY != 0
    posMessageFree(buf);
#endif
    return -E_ARG;
  }
#endif

#if POSCFG_MSG_MEMORY == 0
  mbuf = pos_msgAlloc();
  if (mbuf == NULL)
    return -E_NOMEM;
  mbuf->bufptr = buf;
#else
  mbuf = (MSGBUF_t*) buf;
  POS_ARGCHECK_RET(mbuf, mbuf->magic, POSMAGIC_MSGBUF, -E_ARG); 
#endif

  POS_SCHED_LOCK;
#if POSCFG_FEATURE_EXIT != 0
  if (taskhandle->state != POSTASKSTATE_ACTIVE)
  {
    POS_SCHED_UNLOCK;
#if POSCFG_MSG_MEMORY != 0
    posMessageFree(buf);
#else
    pos_msgFree(mbuf);
#endif
    return -E_FAIL;
  }
#endif
  mbuf->next = NULL;
  if (taskhandle->lastmsg == NULL)
  {
    taskhandle->firstmsg = (void*) mbuf;
    taskhandle->lastmsg = (void*) mbuf;
  }
  else
  {
    ((MSGBUF_t*)(taskhandle->lastmsg))->next = mbuf;
    taskhandle->lastmsg = (void*) mbuf;
  }
  if (taskhandle->msgwait != 0)
  {
    taskhandle->msgwait = 0;
    pos_sched_event((EVENT_t)taskhandle->msgsem);

#if (POSCFG_SOFT_MTASK !=0)&&(SYS_TASKTABSIZE_Y >1)&&(POSCFG_ROUNDROBIN !=0)
    if ((posMustSchedule_g != 0) &&
        (taskhandle->idx_y >= posCurrentTask_g->idx_y))
    {
      pos_schedule();
    }
#else
    pos_schedule();
#endif
  }
  POS_SCHED_UNLOCK;
  return E_OK;
}

/*-------------------------------------------------------------------------*/

#if (POSCFG_SMALLCODE == 0) || (POSCFG_FEATURE_MSGWAIT == 0)
void* posMessageGet(void)
{
  register POSTASK_t task = posCurrentTask_g;
  register MSGBUF_t *mbuf;
  register POSSEMA_t sem;
#if POSCFG_MSG_MEMORY == 0
  void *buf;
#endif
  POS_LOCKFLAGS;

#if POSCFG_ARGCHECK > 1
  if (posInInterrupt_g != 0)
    return NULL;
#endif

  if (task->msgsem == NULL)
  {
    sem = posSemaCreate(0);
    if (sem == NULL)
    {
      return NULL;
    }
    POS_SCHED_LOCK;
    task->msgsem = sem;
  }
  else
  {
    POS_SCHED_LOCK;
  }

  mbuf = (MSGBUF_t*) (task->firstmsg);
  if (mbuf == NULL)
  {
    task->msgwait = 1;
    pos_disableTask(task);
    pos_eventAddTask((EVENT_t)task->msgsem, task);
    pos_schedule();
    mbuf = (MSGBUF_t*) (task->firstmsg);
  }
  task->firstmsg = (void*) (mbuf->next);
  if (task->firstmsg == NULL)
  {
    task->lastmsg = NULL;
  }
  POS_SCHED_UNLOCK;

#if POSCFG_MSG_MEMORY == 0
  buf = mbuf->bufptr;
  pos_msgFree(mbuf);
  return buf;
#else
  return (void*) (mbuf->buffer);
#endif
}
#endif

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_MSGWAIT != 0

void* posMessageWait(UINT_t timeoutticks)
{
  register POSTASK_t task = posCurrentTask_g;
  register MSGBUF_t *mbuf;
  register POSSEMA_t sem;
#if POSCFG_MSG_MEMORY == 0
  void *buf;
#endif
  POS_LOCKFLAGS;

#if POSCFG_ARGCHECK > 1
  if (posInInterrupt_g != 0)
    return NULL;
#endif

  if (task->msgsem == NULL)
  {
    sem = posSemaCreate(0);
    if (sem == NULL)
    {
      return NULL;
    }
    POS_SCHED_LOCK;
    task->msgsem = sem;
  }
  else
  {
    POS_SCHED_LOCK;
  }

  mbuf = (MSGBUF_t*) (task->firstmsg);

  if ((timeoutticks != 0) && (mbuf == NULL))
  {
    if (timeoutticks != INFINITE)
    {
      task->ticks = timeoutticks;
      pos_addToSleepList(task);
    }

    task->msgwait = 1;
    pos_disableTask(task);
    pos_eventAddTask((EVENT_t)task->msgsem, task);
    pos_schedule();
    mbuf = (MSGBUF_t*) (task->firstmsg);

    if (task->msgwait != 0)
    {
      pos_eventRemoveTask((EVENT_t)task->msgsem, task);
      task->msgwait = 0;
    }
    if ((timeoutticks != INFINITE) &&
        (task->prev != task))
    {
      pos_removeFromSleepList(task);
    }
  }

  if (mbuf != NULL)
  {
    task->firstmsg = (void*) (mbuf->next);
    if (task->firstmsg == NULL)
    {
      task->lastmsg = NULL;
    }
    POS_SCHED_UNLOCK;
#if POSCFG_MSG_MEMORY == 0
    buf = mbuf->bufptr;
    pos_msgFree(mbuf);
    return buf;
#else
    return (void*) (mbuf->buffer);
#endif
  }

  POS_SCHED_UNLOCK;
  return NULL;
}

#endif  /* POSCFG_FEATURE_MSGWAIT */

/*-------------------------------------------------------------------------*/

VAR_t posMessageAvailable(void)
{
  return (posCurrentTask_g->firstmsg != NULL) ? 1 : 0;
}

#endif  /* POSCFG_FEATURE_MSGBOXES */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  TIMER
 *-------------------------------------------------------------------------*/

#if (POSCFG_FEATURE_JIFFIES != 0) && (POSCFG_FEATURE_LARGEJIFFIES != 0)
JIF_t posGetJiffies(void)
{
  register JIF_t  jif;
  POS_LOCKFLAGS;
 
  POS_SCHED_LOCK;
  jif = pos_jiffies_g;
  POS_SCHED_UNLOCK;
  return jif;
}
#endif  /* POSCFG_FEATURE_JIFFIES */

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_TIMER != 0

POSTIMER_t posTimerCreate(void)
{
  register TIMER_t  *t;
  POS_LOCKFLAGS;

  POS_SCHED_LOCK;
  t = posFreeTimer_g;

#if SYS_POSTALLOCATE != 0
  if (t == NULL)
  {
    POS_SCHED_UNLOCK;
    t = (TIMER_t*) POS_MEM_ALLOC(sizeof(TIMER_t) +
                                 (POSCFG_ALIGNMENT - 1));
    if (t == NULL)
      return NULL;

    t = MEMALIGN(TIMER_t*, t);
#if POSCFG_ARGCHECK > 1
    t->magic = POSMAGIC_TIMER;
#endif
  }
  else
  {
    posFreeTimer_g = t->next;
    POS_SCHED_UNLOCK;
  }
#else /* SYS_POSTALLOCATE */
  if ((t == NULL)
#if POSCFG_ARGCHECK > 1
      || (t->magic != POSMAGIC_TIMER)
#endif
     )
  {
    POS_SCHED_UNLOCK;
    return NULL;
  }
  posFreeTimer_g = t->next;
  POS_SCHED_UNLOCK;
#endif /* SYS_POSTALLOCATE */
  t->prev   = t;
#if POSCFG_ARGCHECK > 1
  t->wait   = 0;
  t->reload = 0;
#endif
#if POSCFG_FEATURE_TIMERFIRED != 0
  t->fired  = 0;
#endif
  return (POSTIMER_t) t;
}

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_TIMERDESTROY != 0

void posTimerDestroy(POSTIMER_t tmr)
{
  register TIMER_t  *t = (TIMER_t*) tmr;
  POS_LOCKFLAGS;

  POS_ARGCHECK(t, t->magic, POSMAGIC_TIMER); 
  posTimerStop(tmr);
  POS_SCHED_LOCK;
  if (t == t->prev)
  {
    t->next = posFreeTimer_g;
    posFreeTimer_g = t;
  }
  POS_SCHED_UNLOCK;
}

#endif  /* POSCFG_FEATURE_TIMERDESTROY */

/*-------------------------------------------------------------------------*/

VAR_t posTimerSet(POSTIMER_t tmr, POSSEMA_t sema,
                  UINT_t waitticks, UINT_t periodticks)
{
  register TIMER_t  *t = (TIMER_t*) tmr;
  register EVENT_t  ev = (EVENT_t) sema;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(t, t->magic, POSMAGIC_TIMER, -E_ARG); 
  POS_ARGCHECK_RET(ev, ev->e.magic, POSMAGIC_EVENT, -E_ARG); 
#if POSCFG_ARGCHECK > 1
  if (waitticks == 0)
     return -E_ARG;
#endif

  posTimerStop(tmr);
  POS_SCHED_LOCK;
  t->sema   = sema;
  t->wait   = waitticks;
  t->reload = periodticks;
  POS_SCHED_UNLOCK;
  return E_OK;
}

/*-------------------------------------------------------------------------*/

VAR_t posTimerStart(POSTIMER_t tmr)
{
  register TIMER_t *t = (TIMER_t*) tmr;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(t, t->magic, POSMAGIC_TIMER, -E_ARG); 
  POS_SCHED_LOCK;
  t->counter = t->wait;
  if (t->prev == t)
  {
#if POSCFG_FEATURE_TIMERFIRED != 0
    t->fired = 0;
#endif
    pos_addToTimerList(t);
  }
  POS_SCHED_UNLOCK;
  return E_OK;
}

/*-------------------------------------------------------------------------*/

VAR_t posTimerStop(POSTIMER_t tmr)
{
  register TIMER_t *t = (TIMER_t*) tmr;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(t, t->magic, POSMAGIC_TIMER, -E_ARG); 
  POS_SCHED_LOCK;
  if (t->prev != t)
  {
    pos_removeFromTimerList(t);
  }
  POS_SCHED_UNLOCK;
  return E_OK;
}

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_TIMERFIRED != 0

VAR_t posTimerFired(POSTIMER_t tmr)
{
  register TIMER_t *t = (TIMER_t*) tmr;
  register VAR_t  f;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(t, t->magic, POSMAGIC_TIMER, -E_ARG); 
  POS_SCHED_LOCK;
  f = t->fired;
  t->fired = 0;
  POS_SCHED_UNLOCK;
  return f;
}

#endif  /* POSCFG_FEATURE_TIMERFIRED */

#endif  /* POSCFG_FEATURE_TIMER */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  FLAGS
 *-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_FLAGS != 0

POSFLAG_t posFlagCreate(void)
{
  register EVENT_t ev;

  ev = (EVENT_t) posSemaCreate(0);
  if (ev != NULL)
  {
    ev->e.d.flags = 0;
  }
  return (POSFLAG_t) ev;
}

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_FLAGDESTROY != 0

void posFlagDestroy(POSFLAG_t flg)
{
  posSemaDestroy((POSSEMA_t) flg);
}

#endif  /* POSCFG_FEATURE_FLAGDESTROY */

/*-------------------------------------------------------------------------*/

VAR_t posFlagSet(POSFLAG_t flg, UVAR_t flgnum)
{
  register EVENT_t  ev = (EVENT_t) flg;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(ev, ev->e.magic, POSMAGIC_EVENT, -E_ARG); 
#if POSCFG_ARGCHECK != 0
  if (flgnum >= (MVAR_BITS-1))
    return -E_ARG;
#endif
  POS_SCHED_LOCK;
  ev->e.d.flags |= pos_shift1l(flgnum);
  pos_sched_event(ev);
  POS_SCHED_UNLOCK;
  return E_OK;
}

/*-------------------------------------------------------------------------*/

VAR_t posFlagGet(POSFLAG_t flg, UVAR_t mode)
{
  register EVENT_t  ev = (EVENT_t) flg;
  register POSTASK_t task = posCurrentTask_g;
  register UVAR_t  f;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(ev, ev->e.magic, POSMAGIC_EVENT, -E_ARG); 
#if POSCFG_ARGCHECK > 1
  if ((mode != POSFLAG_MODE_GETSINGLE) && 
      (mode != POSFLAG_MODE_GETMASK))
    return -E_ARG;
#endif
  POS_SCHED_LOCK;
  if (ev->e.d.flags == 0)
  {
    do
    {
      pos_disableTask(task);
      pos_eventAddTask(ev, task);
      pos_schedule();
    }
    while (ev->e.d.flags == 0);
  }
  if (mode == POSFLAG_MODE_GETSINGLE)
  {
    f = POS_FINDBIT(ev->e.d.flags);
    ev->e.d.flags &= ~pos_shift1l(f);
  }
  else
  {
    f = ev->e.d.flags;
    ev->e.d.flags = 0;
  }
  POS_SCHED_UNLOCK;
  return (VAR_t) f;
}

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_FLAGWAIT != 0

VAR_t posFlagWait(POSFLAG_t flg, UINT_t timeoutticks)
{
  register EVENT_t  ev = (EVENT_t) flg;
  register POSTASK_t task = posCurrentTask_g;
  register UVAR_t  f;
  POS_LOCKFLAGS;

  POS_ARGCHECK_RET(ev, ev->e.magic, POSMAGIC_EVENT, -E_ARG); 
  POS_SCHED_LOCK;

  if ((timeoutticks != 0) && (ev->e.d.flags == 0))
  {
    if (timeoutticks != INFINITE)
    {
      task->ticks = timeoutticks;
      pos_addToSleepList(task);
    }

    do
    {
      pos_disableTask(task);
      pos_eventAddTask(ev, task);
      pos_schedule();
    }
    while ((ev->e.d.flags == 0) && 
           ((timeoutticks == INFINITE) || (task->prev != task)));

    if (timeoutticks != INFINITE)
    {
      pos_eventRemoveTask(ev, task);
      if (task->prev != task)
      {
        pos_removeFromSleepList(task);
      }
    }
  }
  f = ev->e.d.flags;
  ev->e.d.flags = 0;
  POS_SCHED_UNLOCK;
  return (VAR_t) f;
}

#endif  /* POSCFG_FEATURE_FLAGWAIT */

#endif  /* POSCFG_FEATURE_FLAGS */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  SOFTWARE INTERRUPTS
 *-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_SOFTINTS != 0

void posSoftInt(UVAR_t intno, UVAR_t param)
{
  UVAR_t next;
  POS_LOCKFLAGS;

  if (intno < POSCFG_SOFTINTERRUPTS)
  {
    POS_SCHED_LOCK;
    next = sintIdxIn_g + 1;
    if (next > POSCFG_SOFTINTQUEUELEN)
      next = 0;
    if (next != sintIdxOut_g)
    {
      softintqueue_g[sintIdxIn_g].intno = intno;
      softintqueue_g[sintIdxIn_g].param = param;
      sintIdxIn_g = next;    
    }
    POS_SCHED_UNLOCK;
  }
}

/*-------------------------------------------------------------------------*/

VAR_t posSoftIntSetHandler(UVAR_t intno, POSINTFUNC_t inthandler)
{
  POS_LOCKFLAGS;

  if (intno >= POSCFG_SOFTINTERRUPTS)
    return -E_ARG;
  POS_SCHED_LOCK;
  if (softIntHandlers_g[intno] != NULL)
  {
    POS_SCHED_UNLOCK;
    return -E_FAIL;
  }
  softIntHandlers_g[intno] = inthandler;
  POS_SCHED_UNLOCK;
  return E_OK;
}

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_SOFTINTDEL != 0

VAR_t posSoftIntDelHandler(UVAR_t intno)
{
  POS_LOCKFLAGS;

  if (intno >= POSCFG_SOFTINTERRUPTS)
    return -E_ARG;
  POS_SCHED_LOCK;
  softIntHandlers_g[intno] = NULL;
  POS_SCHED_UNLOCK;
  return E_OK;
}

#endif /* POSCFG_FEATURE_SOFTINTDEL */

#endif /* POSCFG_FEATURE_SOFTINTS */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  ATOMIC VARIABLES
 *-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_ATOMICVAR != 0

void posAtomicSet(POSATOMIC_t *var, INT_t value)
{
  POS_LOCKFLAGS;

  if (var != NULL)
  {
    POS_SCHED_LOCK;
    *var = value;
    POS_SCHED_UNLOCK;
  }
}

/*-------------------------------------------------------------------------*/

INT_t posAtomicGet(POSATOMIC_t *var)
{
  INT_t value;
  POS_LOCKFLAGS;

  if (var == NULL)
    return 0;

  POS_SCHED_LOCK;
  value = *var;
  POS_SCHED_UNLOCK;
  return value;
}

/*-------------------------------------------------------------------------*/

INT_t posAtomicAdd(POSATOMIC_t *var, INT_t value)
{
  INT_t lastval;
  POS_LOCKFLAGS;

  if (var == NULL)
    return 0;

  POS_SCHED_LOCK;
  lastval = *var;
  *var += value;
  POS_SCHED_UNLOCK;
  return lastval;
}

/*-------------------------------------------------------------------------*/

INT_t posAtomicSub(POSATOMIC_t *var, INT_t value)
{
  INT_t lastval;
  POS_LOCKFLAGS;

  if (var == NULL)
    return 0;

  POS_SCHED_LOCK;
  lastval = *var;
  *var -= value;
  POS_SCHED_UNLOCK;
  return lastval;
}

#endif /* POSCFG_FEATURE_ATOMICVAR */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  LISTS
 *-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_LISTS != 0

void posListAdd(POSLISTHEAD_t *listhead, UVAR_t pos, POSLIST_t *new)
{
  register POSLIST_t *next, *prev;
  POS_LOCKFLAGS;

  POS_SCHED_LOCK;
  if (pos == POSLIST_HEAD)
  {
    next = listhead->next;
    prev = (POSLIST_t*) listhead;
  }
  else
  {
    next = (POSLIST_t*) listhead;
    prev = listhead->prev;
  }
  next->prev = new;
  new->next  = next;
  new->prev  = prev;
  prev->next = new;
  listhead->length++;
  if (listhead->flag != 0)
  {
    listhead->flag = 0;
    POS_SCHED_UNLOCK;
    posSemaSignal(listhead->sema);
  }
  else
  {
    POS_SCHED_UNLOCK;
  }
}

/*-------------------------------------------------------------------------*/

POSLIST_t* posListGet(POSLISTHEAD_t *listhead, UVAR_t pos,
                      UINT_t timeout)
{
  register POSLIST_t *elem;
#if POSCFG_FEATURE_SEMAWAIT != 0
  register VAR_t status;
  POS_LOCKFLAGS;

  POS_SCHED_LOCK;
  for (;;)
  {  
#else
  POS_LOCKFLAGS;

  for (;;)
  {  
    POS_SCHED_LOCK;
#endif
    if (pos == POSLIST_HEAD)
    {
      elem = listhead->next;
    }
    else
    {
      elem = listhead->prev;
    }
    if (elem == (POSLIST_t*) listhead)
    {
      if ((timeout == 0) || (posInInterrupt_g != 0))
      {
        POS_SCHED_UNLOCK;
        return NULL;
      }
      if (listhead->sema == NULL)
      {
        POS_SCHED_UNLOCK;
        listhead->sema = posSemaCreate(0);
        if (listhead->sema == NULL)
          return NULL;
#if POSCFG_FEATURE_SEMAWAIT != 0
        POS_SCHED_LOCK;
#endif
        continue;
      }
      listhead->flag = 1;
      POS_SCHED_UNLOCK;
#if POSCFG_FEATURE_SEMAWAIT != 0
      status = posSemaWait(listhead->sema, timeout);
      POS_SCHED_LOCK;
      listhead->flag = 0;
      if (status != E_OK)
      {
        POS_SCHED_UNLOCK;
        return NULL;
      }
#else
      if (posSemaGet(listhead->sema) != E_OK)
        return NULL;
#endif
    }
    else
    {
      pos_listRemove(elem);
      listhead->length--;
      POS_SCHED_UNLOCK;
      return elem;
    }
  }
}

/*-------------------------------------------------------------------------*/

void posListRemove(POSLIST_t *listelem)
{
  POS_LOCKFLAGS;
  POS_SCHED_LOCK;
  if (listelem != NULL)
  {
    pos_listRemove(listelem);
    listelem->head->length--;
  }
  POS_SCHED_UNLOCK;
}

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_LISTJOIN != 0

void posListJoin(POSLISTHEAD_t *baselisthead, UVAR_t pos,
                 POSLISTHEAD_t *joinlisthead)
{
  register POSLIST_t *elem;
  POS_LOCKFLAGS;

  POS_SCHED_LOCK;
  if (POSLIST_IS_EMPTY(joinlisthead))
  {
    POS_SCHED_UNLOCK;
  }
  else
  {
    POSLIST_FOR_EACH_ENTRY(joinlisthead, elem)
    {
      elem->head = baselisthead;
    }
    elem = joinlisthead->next;
    pos_listRemove((POSLIST_t*)joinlisthead);
    if (pos == POSLIST_HEAD)
    {
      pos_listJoin((POSLIST_t*)baselisthead, baselisthead->next, elem);
    }
    else
    {
      pos_listJoin(baselisthead->prev, (POSLIST_t*)baselisthead, elem);
    }
    baselisthead->length += joinlisthead->length;
    joinlisthead->prev = (POSLIST_t*) joinlisthead;
    joinlisthead->next = (POSLIST_t*) joinlisthead;
    joinlisthead->length = 0;
    if (baselisthead->flag != 0)
    {
      baselisthead->flag = 0;
      POS_SCHED_UNLOCK;
      posSemaSignal(baselisthead->sema);
    }
    else
    {
      POS_SCHED_UNLOCK;
    }
  }
}

#endif /* POSCFG_FEATURE_LISTJOIN */

/*-------------------------------------------------------------------------*/

UINT_t posListLen(POSLISTHEAD_t *listhead)
{
  UINT_t len;
  POS_LOCKFLAGS;

  if (listhead == NULL)
    return 0;

  POS_SCHED_LOCK;
  len = listhead->length;
  POS_SCHED_UNLOCK;
  return len;
}

/*-------------------------------------------------------------------------*/

void posListInit(POSLISTHEAD_t *listhead)
{
  if (listhead != NULL)
  {
    listhead->prev = (POSLIST_t*) listhead;
    listhead->next = (POSLIST_t*) listhead;
    listhead->length = 0;
    listhead->sema = NULL;
    listhead->flag = 0;
  }
}

/*-------------------------------------------------------------------------*/

void posListTerm(POSLISTHEAD_t *listhead)
{
  if (listhead != NULL)
  {
    if (listhead->sema != NULL)
    {
      posSemaDestroy(listhead->sema);
      listhead->flag = 0;
      listhead->sema = NULL;
    }
  }
}

#endif /* POSCFG_FEATURE_LISTS */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTION:  INSTALL IDLE HOOK FUNCTION
 *-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_IDLETASKHOOK != 0

POSIDLEFUNC_t  posInstallIdleTaskHook(POSIDLEFUNC_t idlefunc)
{
  POSIDLEFUNC_t  prevhook;
  POS_LOCKFLAGS;

  POS_SCHED_LOCK;
  prevhook = posIdleTaskFuncHook_g;
  posIdleTaskFuncHook_g = idlefunc;
  POS_SCHED_UNLOCK;
  return prevhook;
}

#endif /* POSCFG_FEATURE_IDLETASKHOOK */



/*---------------------------------------------------------------------------
 * EXPORTED FUNCTIONS:  OPERATING SYSTEM INITIALIZATION
 *-------------------------------------------------------------------------*/

#if POSCFG_TASKSTACKTYPE == 0
void posInit(POSTASKFUNC_t firstfunc, void *funcarg, VAR_t priority,
             void *stackFirstTask, void *stackIdleTask)
#elif POSCFG_TASKSTACKTYPE == 1
void  posInit(POSTASKFUNC_t firstfunc, void *funcarg, VAR_t priority,
              UINT_t taskStackSize, UINT_t idleStackSize)
#else
void  posInit(POSTASKFUNC_t firstfunc, void *funcarg, VAR_t priority)
#endif
{
  POSTASK_t  task;
#if SYS_FEATURE_EVENTS != 0
  EVENT_t   ev;
#endif
#if (POSCFG_FEATURE_MSGBOXES != 0) && (POSCFG_MAX_MESSAGES != 0)
  MSGBUF_t  *mbuf;
#endif
#if (POSCFG_FEATURE_TIMER != 0) && (POSCFG_MAX_TIMER != 0)
  TIMER_t   *tmr;
#endif
#if POSCFG_DYNAMIC_MEMORY != 0
  void      *m;
#endif
  UVAR_t   i;
  POS_LOCKFLAGS;

#if POSCFG_CALLINITARCH != 0
  p_pos_initArch();
#endif

#if POSCFG_DYNAMIC_MEMORY != 0
  (void) m;
  (void) task;
  (void) ev;

#if POSCFG_MAX_TASKS != 0
  m = POS_MEM_ALLOC(ALIGNEDBUFSIZE(sizeof(struct POSTASK),
                    POSCFG_MAX_TASKS));
#if POSCFG_ARGCHECK > 1
  if (m == NULL)
    return;
#endif
  posFreeTasks_g = MEMALIGN(POSTASK_t, m);
#else
  posFreeTasks_g = NULL;
#endif

#if SYS_FEATURE_EVENTS != 0
#if (POSCFG_MAX_EVENTS + SYS_MSGBOXEVENTS) != 0
  m = POS_MEM_ALLOC(ALIGNEDBUFSIZE(sizeof(union EVENT), 
                    (POSCFG_MAX_EVENTS + SYS_MSGBOXEVENTS)));
#if POSCFG_ARGCHECK > 1
  if (m == NULL)
    return;
#endif
  posFreeEvents_g = MEMALIGN(EVENT_t, m);
#else
  posFreeEvents_g = NULL;
#endif
#endif

#if POSCFG_FEATURE_MSGBOXES != 0
#if POSCFG_MAX_MESSAGES != 0
  m = POS_MEM_ALLOC(ALIGNEDBUFSIZE(sizeof(MSGBUF_t), POSCFG_MAX_MESSAGES));
#if POSCFG_ARGCHECK > 1
  if (m == NULL)
    return;
#endif
  posFreeMessagebuf_g = MEMALIGN(MSGBUF_t*, m);
#else
  posFreeMessagebuf_g = NULL;
#endif
#endif

#if POSCFG_FEATURE_TIMER != 0
#if POSCFG_MAX_TIMER != 0
  m = POS_MEM_ALLOC(ALIGNEDBUFSIZE(sizeof(TIMER_t), POSCFG_MAX_TIMER));
#if POSCFG_ARGCHECK > 1
  if (m == NULL)
    return;
#endif
  posFreeTimer_g = MEMALIGN(TIMER_t*, m);
#else
  posFreeTimer_g = NULL;
#endif
#endif

#else /* POSCFG_DYNAMIC_MEMORY */

#if POSCFG_MAX_TASKS != 0
  posFreeTasks_g = MEMALIGN(POSTASK_t, posStaticTaskMem_g);
#else
  posFreeTasks_g = NULL;
#endif

#if SYS_FEATURE_EVENTS != 0
#if (POSCFG_MAX_EVENTS + SYS_MSGBOXEVENTS) != 0
  posFreeEvents_g = MEMALIGN(EVENT_t, posStaticEventMem_g);
#else
  posFreeEvents_g = NULL;
#endif
#endif

#if POSCFG_FEATURE_MSGBOXES != 0
#if POSCFG_MAX_MESSAGES != 0
  posFreeMessagebuf_g = MEMALIGN(MSGBUF_t*, posStaticMessageMem_g);
#else
  posFreeMessagebuf_g = NULL;
#endif
#endif

#if POSCFG_FEATURE_TIMER != 0
#if POSCFG_MAX_TIMER != 0
  posFreeTimer_g = MEMALIGN(TIMER_t*, posStaticTmrMem_g);
#else
  posFreeTimer_g = NULL;
#endif
#endif

#endif /* POSCFG_DYNAMIC_MEMORY */

#if POSCFG_MAX_TASKS != 0
  task = posFreeTasks_g;
  for (i=0; i<POSCFG_MAX_TASKS-1; ++i)
  {
#if SYS_TASKSTATE != 0
    task->state = POSTASKSTATE_UNUSED;
#endif
    task->next = NEXTALIGNED(POSTASK_t, task);
    task = task->next;
  }
#if SYS_TASKSTATE != 0
  task->state = POSTASKSTATE_UNUSED;
#endif
  task->next = NULL;
#endif
  
#if SYS_FEATURE_EVENTS != 0
#if (POSCFG_MAX_EVENTS + SYS_MSGBOXEVENTS) != 0
  ev = posFreeEvents_g;
#if POSCFG_MAX_EVENTS > 1
  for (i=0; i<POSCFG_MAX_EVENTS-1; ++i)
  {
#if POSCFG_ARGCHECK > 1
    ev->l.magic = POSMAGIC_EVENT;
#endif
    ev->l.next = NEXTALIGNED(EVENT_t, ev);
    ev = ev->l.next;
  }
#endif
#if POSCFG_ARGCHECK > 1
  ev->l.magic = POSMAGIC_EVENT;
#endif
  ev->l.next = NULL;
#endif
#endif

#if POSCFG_FEATURE_MSGBOXES != 0
  msgAllocWaitReq_g = 0;
  msgAllocSyncSem_g = posSemaCreate(1);
  msgAllocWaitSem_g = posSemaCreate(0);
#if ((POSCFG_MAX_EVENTS + SYS_MSGBOXEVENTS) < 2) && (SYS_POSTALLOCATE != 0)
  if ((msgAllocSyncSem_g == NULL) || (msgAllocWaitSem_g == NULL))
    return;
#endif
#if POSCFG_MAX_MESSAGES != 0
  mbuf = posFreeMessagebuf_g;
  for (i=0; i<POSCFG_MAX_MESSAGES-1; ++i)
  {
#if POSCFG_ARGCHECK > 1
    mbuf->magic = POSMAGIC_MSGBUF;
#endif
    mbuf->next = NEXTALIGNED(MSGBUF_t*, mbuf);
    mbuf = mbuf->next;
  }
#if POSCFG_ARGCHECK > 1
  mbuf->magic = POSMAGIC_MSGBUF;
#endif
  mbuf->next = NULL;
#endif
#endif

#if POSCFG_FEATURE_TIMER != 0
  posActiveTimers_g = NULL;
#if POSCFG_MAX_TIMER != 0
  tmr = posFreeTimer_g;
#if POSCFG_MAX_TIMER > 1
  for (i=0; i<POSCFG_MAX_TIMER-1; ++i)
  {
#if POSCFG_ARGCHECK > 1
    tmr->magic = POSMAGIC_TIMER;
#endif
    tmr->next = NEXTALIGNED(TIMER_t*, tmr);
    tmr = tmr->next;
  }
#endif
#if POSCFG_ARGCHECK > 1
  tmr->magic = POSMAGIC_TIMER;
#endif
  tmr->next = NULL;
#endif
#endif

  for (i=0; i<SYS_TASKTABSIZE_Y; ++i)
  {
#if POSCFG_ROUNDROBIN != 0
    posNextRoundRobin_g[i] = 0;
#endif
#if (POSCFG_ROUNDROBIN != 0) && (SYS_TASKTABSIZE_X < MVAR_BITS)
    posAllocatedTasks_g.xtable[i] = (UVAR_t) ~((1 << SYS_TASKTABSIZE_X) - 1);
#else
    posAllocatedTasks_g.xtable[i] = 0;
#endif
    posReadyTasks_g.xtable[i] = 0;
  }
#if SYS_TASKTABSIZE_Y > 1
  posAllocatedTasks_g.ymask = 0;
  posReadyTasks_g.ymask = 0;
#endif

#if POSCFG_FEATURE_SOFTINTS != 0
  sintIdxIn_g = 0;
  sintIdxOut_g = 0;
  for (i=0; i<POSCFG_SOFTINTERRUPTS; i++)
  {
    softIntHandlers_g[i] = NULL;
  }
#endif
#if POSCFG_CTXSW_COMBINE > 1
  posCtxCombineCtr_g = 0;
#endif
#if POSCFG_FEATURE_INHIBITSCHED != 0
  posInhibitSched_g = 0;
#endif
  posMustSchedule_g = 0;
  posInInterrupt_g  = 1;
  posSleepingTasks_g   = NULL;
#if POSCFG_FEATURE_JIFFIES != 0
#if POSCFG_FEATURE_LARGEJIFFIES == 0
  jiffies = 0;
#else
  pos_jiffies_g = 0;
#endif
#endif
#if POSCFG_FEATURE_IDLETASKHOOK != 0
  posIdleTaskFuncHook_g = NULL;
#endif

#if POSCFG_TASKSTACKTYPE == 0
  posTaskCreate(pos_idletask, NULL, 0, stackIdleTask);
#elif POSCFG_TASKSTACKTYPE == 1
  posTaskCreate(pos_idletask, NULL, 0, idleStackSize);
#else
  posTaskCreate(pos_idletask, NULL, 0);
#endif

  /* start mutlitasking */
  posNextTask_g = posTaskCreate(firstfunc, funcarg,
#if POSCFG_TASKSTACKTYPE == 0
                priority, stackFirstTask);
#elif POSCFG_TASKSTACKTYPE == 1
                priority, taskStackSize);
#else
                priority);
#endif
  POS_SCHED_LOCK;
  posCurrentTask_g  = posNextTask_g;
  posRunning_g      = 1;
  posInInterrupt_g  = 0;
  p_pos_startFirstContext();
  for(;;);
}

/*-------------------------------------------------------------------------*/

