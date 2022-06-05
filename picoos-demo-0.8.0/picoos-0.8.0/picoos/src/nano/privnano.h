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
 *  This file is part of the pico]OS realtime operating system.
 *
 *  $Id: privnano.h,v 1.1 2004/03/16 21:33:39 dkuschel Exp $
 *
 */

#ifndef _PRIVNANO_H
#define _PRIVNANO_H
#define NANOINTERNAL

#if NOSCFG_MEM_MANAGER_TYPE == 0
#include <stdlib.h>
#endif

#define PICOS_PRIVINCL
#include <pos_nano.h>

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_INHIBITSCHED == 0
#error POSCFG_FEATURE_INHIBITSCHED not enabled!
#endif

/*-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_MEMALLOC != 0
void nos_initMem(void);
#endif

/*-------------------------------------------------------------------------*/

#if (NOSCFG_FEATURE_CONIN != 0) || (NOSCFG_FEATURE_CONOUT != 0)
void nos_initConIO(void);
#endif

#if NOSCFG_FEATURE_BOTTOMHALF != 0
void nos_initBottomHalfs(void);
#endif

/*-------------------------------------------------------------------------*/

#endif /* _PRIVNANO_H */
