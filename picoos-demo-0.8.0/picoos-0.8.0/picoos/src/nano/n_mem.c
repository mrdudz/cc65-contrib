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
 * @file   n_mem.c
 * @brief  nano layer, memory management
 * @author Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: n_mem.c,v 1.2 2004/03/21 18:34:38 dkuschel Exp $
 */

#define _N_MEM_C
#include "../src/nano/privnano.h"

/* check features */
#if NOSCFG_FEATURE_MEMALLOC != 0
#if POSCFG_FEATURE_INHIBITSCHED == 0
#error POSCFG_FEATURE_INHIBITSCHED not enabled
#endif
#endif

/* include malloc from runtime library */
#if (NOSCFG_MEM_MANAGER_TYPE == 0)
#ifdef NULL
#undef NULL
#endif
#include <stdlib.h>
#endif



/*---------------------------------------------------------------------------
 *  MEMORY FUNCTIONS:  memset / memcpy
 *-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_MEMSET != 0

void nosMemSet(void *buf, char val, UINT_t count)
{
  char   *cb = (char*) buf;

#if (MVAR_BITS > 8) && (POSCFG_SMALLCODE == 0) && (POSCFG_FASTCODE != 0)
  UVAR_t i;

  i = (UVAR_t) (~(((MEMPTR_t) buf)-1)) & (sizeof(UVAR_t)-1);
  if ((i != 0) && ((UINT_t)i < count))
  {
    count -= (UINT_t) i;
#if (MVAR_BITS > 16)
    while (i != 0)
    {
#endif
      *cb++ = val;
#if (MVAR_BITS > 16)
      i--;
    }
#endif
  }

  if (count >= sizeof(UVAR_t))
  {
    UVAR_t f = (UVAR_t) val;
    f |= f << 8;
#if MVAR_BITS > 16
    f |= f << 16;
#endif
    do
    {
      *((UVAR_t*)cb)++ = f;
      count -= sizeof(UVAR_t);
    }
    while (count >= sizeof(UVAR_t));
  }

  if (count != 0)
  {
#if (MVAR_BITS > 16)
    do
    {
#endif
      *cb = val;
#if (MVAR_BITS > 16)
      cb++;
      count--;
    }
    while (count != 0);
#endif
  }

#else

  while (count != 0)
  {
    *cb = val;
    cb++;
    count--;
  }

#endif
}

#endif /* NOSCFG_FEATURE_MEMSET */

/*-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_MEMCOPY != 0

void nosMemCopy(void *dst, void *src, UINT_t count)
{
  char   *cd = (char*) dst;
  char   *cs = (char*) src;

#if (MVAR_BITS > 8) && (POSCFG_SMALLCODE == 0) && (POSCFG_FASTCODE != 0)
  UVAR_t i;

  i = (UVAR_t) (~(((MEMPTR_t)src)-1)) & (sizeof(UVAR_t)-1);
  if ((i == (UVAR_t)((~(((MEMPTR_t)dst)-1)) & (sizeof(UVAR_t)-1))) &&
      (count > sizeof(UVAR_t)))
  {
    if (i != 0)
    {
      count -= (UINT_t) i;
#if (MVAR_BITS > 16)
      do
      {
#endif
        *cd++ = *cs++;
#if (MVAR_BITS > 16)
        --i;
      }
      while (i != 0);
#endif
    }

    if (count >= sizeof(UVAR_t))
    {
      do
      {
        *((UVAR_t*)cd)++ = *((UVAR_t*)cs)++;
        count -= sizeof(UVAR_t);
      }
      while (count >= sizeof(UVAR_t));
    }
  }

#endif

  while (count != 0)
  {
    *cd++ = *cs++;
    --count;
  }
}

#endif /* NOSCFG_FEATURE_MEMCOPY */




/*---------------------------------------------------------------------------
 *
 *                          NANO MEMORY ALLOCATOR
 *
 * Notes:
 *   This memory allocator was developed for small devices. It is fast as
 *   long the memory heap is not too fragmented (that means, malloc and free
 *   is not called too often; this should be true for small applications on
 *   small devices).  I recommend to use the memory allocator from the
 *   runtime library on bigger devices (16 bit CPUs and above).
 *
 *-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_MEMALLOC != 0
#if NOSCFG_MEM_MANAGER_TYPE == 1

#define MEM_MAGIC    0x5D7A
#define MEM_ALIGN(x) (((x) + (POSCFG_ALIGNMENT-1)) & ~(POSCFG_ALIGNMENT - 1))
#define BLOCK_STRUCT_SIZE    MEM_ALIGN(sizeof(struct BLOCK_s))

typedef struct BLOCK_s
{
  union {
    struct BLOCK_s  *next;
    unsigned int    magic;
  } h;
  unsigned int  size;
} *BLOCK_t;

static BLOCK_t  freeBlockList_g;

/*-------------------------------------------------------------------------*/

void* nos_malloc(unsigned int size)
{
  BLOCK_t  bp, bl, l, p;
  unsigned int s;

  if (size == 0)
    return NULL;

#if POSCFG_ALIGNMENT > 1
  size = MEM_ALIGN(size);
#endif
  size += BLOCK_STRUCT_SIZE;

  /* find block that fits best
   */

  bp = NULL;
  l  = NULL;
  s  = (unsigned int) -1;

  for (p = freeBlockList_g; p != NULL; p = p->h.next)
  {
    if ((p->size >= size) && (p->size < s))
    {
      bp = p;
      bl = l;
      s  = p->size;
      if (size == s)
        break;
    }
    l = p;
  }

  if (bp == NULL)
    return NULL;

  s = bp->size - size;
  if (s >= (BLOCK_STRUCT_SIZE + POSCFG_ALIGNMENT))
  {
    /* take memory from top of the block and adjust the size */
    bp->size = s;
    bp = (BLOCK_t) (((MEMPTR_t) bp) + s);
    bp->size = size;
  }
  else
  {
    /* take the whole block */
    if (bl == NULL)
    {
      freeBlockList_g = bp->h.next;
    }
    else
    {
      bl->h.next = bp->h.next;
    }
  }

  bp->h.magic = MEM_MAGIC;
  return (void*) (bp + 1);
}

/*-------------------------------------------------------------------------*/

void nos_free(void *mp)
{
  BLOCK_t b, p;
#if NOSCFG_MEM_MANAGE_MODE == 1
  BLOCK_t n, lp = NULL, lb = NULL;
  short f = 0;
#endif

  if (((MEMPTR_t) mp) < BLOCK_STRUCT_SIZE)
    return;

  b = ((BLOCK_t) mp) - 1;

  /* test magic number and prevent memory block from beeing freed twice */
  if (b->h.magic != MEM_MAGIC)
    return;
  b->h.magic = 0;

  /* find neighbour blocks and join them */

  /* find left neighbour */
  for (p = freeBlockList_g; p != NULL; p = p->h.next)
  {
    if ((BLOCK_t)(void*)(((MEMPTR_t) p) + p->size) == b)
    {
      p->size += b->size;
#if NOSCFG_MEM_MANAGE_MODE == 1
      b = p;
      f = 1;
      break;
    }
    lb = p;
#else
      return;
    }
#endif
  }

  /* find right neighbour */
#if NOSCFG_MEM_MANAGE_MODE == 1
  n = (BLOCK_t)(void*)(((MEMPTR_t) b) + b->size);
  for (p = freeBlockList_g; p != NULL; p = p->h.next)
  {
    if (p != b)  /* Skip if p == b. This is needed to get the correct lp */
    {
      if (p == n)  /* found right neigbour? */
      {
        /* unchain b from list again if it was just placed there */
        if (f)
        {
          if (lb == NULL)
          {
            freeBlockList_g = b->h.next;
            if (freeBlockList_g == p)
            {
              lp = NULL;
            }
          }
          else
          {
            lb->h.next = b->h.next;
          }
        }

        /* Increase the size of b by the size of p */
        b->size += p->size;

        /* chain b, and replace p in list */
        b->h.next = p->h.next;
        if (lp == NULL)
        {
          freeBlockList_g = b;
        }
        else
        {
          lp->h.next = b;
        }

        f = 1;
        break;
      }
      lp = p;
    }
  }

  if (!f)
#endif
  {
    /* could not join blocks, simply add to list */
    b->h.next = freeBlockList_g;
    freeBlockList_g = b;
  }
}

/*-------------------------------------------------------------------------*/

void nos_initMem(void)
{
  freeBlockList_g = (BLOCK_t)(void*)MEM_ALIGN((MEMPTR_t)(__heap_start));
  freeBlockList_g->h.next = NULL;
  freeBlockList_g->size =
    ((MEMPTR_t)__heap_end) - ((MEMPTR_t)freeBlockList_g) + 1;
}

#endif  /* NOSCFG_MEM_MANAGER_TYPE == 1 */

/*-------------------------------------------------------------------------*/

void* nosMemAlloc(UINT_t size)
{
  void *p;
  if (posRunning_g == 0)
    return NOS_MEM_ALLOC(size);
  posTaskSchedLock();
  p = NOS_MEM_ALLOC(size);
  posTaskSchedUnlock();
  return p;
}

/*-------------------------------------------------------------------------*/

void  nosMemFree(void *p)
{
  posTaskSchedLock();
  NOS_MEM_FREE(p);
  posTaskSchedUnlock();
}

/*-------------------------------------------------------------------------*/

#else  /* NOSCFG_FEATURE_MEMALLOC != 0 */

/* this is just a dummy function */
void nos_initMem(void) {}

#endif


