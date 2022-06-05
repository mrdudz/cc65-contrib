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
 * @file    pos_nano.h
 * @brief   pico]OS nano layer main include file
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: pos_nano.h,v 1.2 2004/03/21 18:36:29 dkuschel Exp $
 */

#ifndef _POS_NANO_H
#define _POS_NANO_H

#ifndef DOX
#define DOX  0
#endif



/*---------------------------------------------------------------------------
 *  NANO LAYER INSTALLATION
 *-------------------------------------------------------------------------*/

/* include nano configuration file */
#include <noscfg.h>

#if POSCFG_TASKSTACKTYPE == 0

/* define hook */
#ifdef POSCFG_STKFREE_HOOK
#undef POSCFG_STKFREE_HOOK
#endif
#define POSCFG_STKFREE_HOOK  1

/* set additional task data for the nano layer */
#define NOS_TASKDATA  void *nosstkroot;

#endif /* POSCFG_TASKSTACKTYPE == 0 */

/* include pico]OS header if not yet done */
#ifndef _PICOOS_H
#include <picoos.h>
#endif



/*---------------------------------------------------------------------------
 *  CONFIGURATION
 *-------------------------------------------------------------------------*/

#ifndef NOSCFG_FEATURE_MEMALLOC
#error  NOSCFG_FEATURE_MEMALLOC not defined
#endif
#ifndef NOSCFG_FEATURE_CONIN
#error  NOSCFG_FEATURE_CONIN not defined
#endif
#ifndef NOSCFG_FEATURE_CONOUT
#error  NOSCFG_FEATURE_CONOUT not defined
#endif
#ifndef NOSCFG_CONOUT_HANDSHAKE
#error  NOSCFG_CONOUT_HANDSHAKE not defined
#endif
#ifndef NOSCFG_CONOUT_FIFOSIZE
#error  NOSCFG_CONOUT_FIFOSIZE not defined
#endif
#ifndef NOSCFG_FEATURE_PRINTF
#error  NOSCFG_FEATURE_PRINTF not defined
#endif
#ifndef NOSCFG_FEATURE_SPRINTF
#error  NOSCFG_FEATURE_SPRINTF not defined
#endif
#ifndef NOSCFG_FEATURE_TASKCREATE
#error  NOSCFG_FEATURE_TASKCREATE not defined
#endif
#ifndef NOSCFG_DEFAULT_STACKSIZE
#error  NOSCFG_DEFAULT_STACKSIZE not defined in the port configuration file
#endif
#ifndef NOSCFG_STACK_GROWS_UP
#error  NOSCFG_STACK_GROWS_UP not defined in the port configuration file
#endif
#ifndef NOSCFG_MEM_MANAGE_MODE
#error  NOSCFG_MEM_MANAGE_MODE not defined
#endif
#ifndef NOSCFG_FEATURE_MEMSET
#error  NOSCFG_FEATURE_MEMSET not defined
#endif
#ifndef NOSCFG_FEATURE_MEMCOPY
#error  NOSCFG_FEATURE_MEMCOPY not defined
#endif
#ifndef NOSCFG_STKMEM_RESERVE
#error  NOSCFG_STKMEM_RESERVE not defined
#endif
#if NOSCFG_MEM_MANAGER_TYPE == 2
#ifndef NOSCFG_MEM_USER_MALLOC
#error  NOSCFG_MEM_USER_MALLOC not defined
#endif
#ifndef NOSCFG_MEM_USER_FREE
#error  NOSCFG_MEM_USER_FREE not defined
#endif
#endif
#ifndef NOSCFG_FEATURE_BOTTOMHALF
#error  NOSCFG_FEATURE_BOTTOMHALF not defined
#endif
#if NOSCFG_FEATURE_BOTTOMHALF != 0
#ifndef NOS_MAX_BOTTOMHALFS
#error  NOS_MAX_BOTTOMHALFS not defined
#endif
#if (NOS_MAX_BOTTOMHALFS == 0) || (NOS_MAX_BOTTOMHALFS > MVAR_BITS)
#error NOS_MAX_BOTTOMHALFS must be in the range 1 .. MVAR_BITS
#endif
#endif
#ifndef NOS_FEATURE_CPUUSAGE
#error  NOS_FEATURE_CPUUSAGE not defined
#endif



/*---------------------------------------------------------------------------
 *  DATA TYPES
 *-------------------------------------------------------------------------*/

#if (DOX!=0) || (NOSCFG_FEATURE_BOTTOMHALF != 0)
/** Bottom half function pointer.
 * @param   arg         Optional argument that was set when the
 *                      bottom half was registered with
 *                      ::nosBottomHalfRegister.
 * @param   bh          Number of the bottom half
 *                      (0 .. ::NOS_MAX_BOTTOMHALFS - 1)
 */
typedef void (*NOSBHFUNC_t)(void* arg, UVAR_t bh);
#endif



/*---------------------------------------------------------------------------
 *  MEMORY MANAGEMENT
 *-------------------------------------------------------------------------*/

/** @defgroup mem Memory Management
 * @ingroup userapin
 * 
 * <b> Note: This API is part of the nano layer </b>
 *
 * The nano layer supplies a set of memory management functions.
 * pico]OS functions are thread save, thus pico]OS can replace the
 * memory allocation functions of a runtime library that was not
 * designed for a multitasking environment.
 * @{
 */

#if (DOX!=0) || (NOSCFG_FEATURE_MEMALLOC != 0)

/**
 * Allocate memory from the heap.
 * This function allocates a block of memory from the heap.
 * The function is thread save, thus multiple threads can access the
 * heap without corrupting it.
 * @param   size in bytes of the memory block to allocate.
 * @return  The function returns the pointer to the new memory block
 *          on success. NULL is returned when the function failed
 *          to allocate a block with the wished size.
 * @note    ::NOSCFG_FEATURE_MEMALLOC must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosMemFree, NOSCFG_MEM_MANAGER_TYPE
 */
void* nosMemAlloc(UINT_t size);

/**
 * Free a block of memory to the heap.
 * This function is the counterpart to ::nosMemAlloc.
 * @param   p pointer to the memory block to free.
 * @note    ::NOSCFG_FEATURE_MEMALLOC must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosMemAlloc, NOSCFG_MEM_MANAGER_TYPE
 */
void  nosMemFree(void *p);

/* overwrite standard memory allocation functions */
#ifndef NANOINTERNAL
#ifdef malloc
#undef malloc
#endif
#ifdef calloc
#undef calloc
#endif
#ifdef free
#undef free
#endif
#define malloc  nosMemAlloc
#define calloc  (not_supported)
#define free    nosMemFree
#else /* NANOINTERNAL */
/* internal malloc/free, used by OS core and platform ports */
#if NOSCFG_MEM_MANAGER_TYPE == 0
#define NOS_MEM_ALLOC(x)   malloc((size_t)(x))
#define NOS_MEM_FREE(x)    free(x)
#elif   NOSCFG_MEM_MANAGER_TYPE == 1
void*   nos_malloc(unsigned int size);
void    nos_free(void *mp);
#define NOS_MEM_ALLOC(x)   nos_malloc(x)
#define NOS_MEM_FREE(x)    nos_free(x)
#elif   NOSCFG_MEM_MANAGER_TYPE == 2
#define NOS_MEM_ALLOC(x)   NOSCFG_MEM_USER_MALLOC(x)
#define NOS_MEM_FREE(x)    NOSCFG_MEM_USER_FREE(x)
#endif
#endif /* NANOINTERNAL */

#endif /* NOSCFG_FEATURE_MEMALLOC */

#if (DOX!=0) || (NOSCFG_FEATURE_MEMSET != 0)

/**
 * Fill a block of memory with a special character.
 * This function works like the memset function from the
 * C runtime library.
 * @param   buf  pointer to the destination memory block
 * @param   val  character to fill into the memory block
 * @param   count  number of bytes to fill into the block
 * @note    ::NOSCFG_FEATURE_MEMSET must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosMemAlloc, nosMemCopy
 */
void nosMemSet(void *buf, char val, UINT_t count);

#ifdef memset
#undef memset
#endif
#define memset  nosMemSet

#endif /* NOSCFG_FEATURE_MEMSET */

#if (DOX!=0) || (NOSCFG_FEATURE_MEMCOPY != 0)

/**
 * Copy a block of memory.
 * This function works like the memcpy function from the
 * C runtime library.
 * @param   dst  pointer to the destination memory block
 * @param   src  pointer to the source memory block
 * @param   count  number of bytes to copy
 * @note    ::NOSCFG_FEATURE_MEMCOPY must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosMemAlloc, nosMemSet
 */
void nosMemCopy(void *dst, void *src, UINT_t count);

#ifdef memcpy
#undef memcpy
#endif
#define memcpy  nosMemCopy

#endif /* NOSCFG_FEATURE_MEMCOPY */

/** @} */



/*---------------------------------------------------------------------------
 *  CONSOLE INPUT / OUTPUT
 *-------------------------------------------------------------------------*/

/** @defgroup conio Console Input / Output
 * @ingroup userapin
 * 
 * <b> Note: This API is part of the nano layer </b>
 *
 * The nano layer supplies a set of multitasking able console I/O functions.
 * Note that the platform port must support some basic I/O mechanisms.
 * For console output, pico]OS calls the function ::p_putchar to output
 * a single character. This function may fail when a transmitter FIFO
 * ran out of space, and the function ::c_nos_putcharReady should be called
 * when the transmitter is ready again. Input from a terminal or keyboard is
 * fet into pico]OS by calling the function ::c_nos_keyinput or by rising the
 * software interrupt number zero with the keycode as parameter. Not all
 * platform ports may support console I/O, please read the port documentation
 * for further information.<br>
 * Since the nano layer supplies also a set of printf and sprintf functions,
 * you may no more need a large runtime library in some special cases.
 * @{
 */
#if (DOX!=0) || (NOSCFG_FEATURE_CONIN != 0)

/**
 * Keyboard input.
 * This function is called by the architecture port to feed keyboard
 * input into the nano layer.
 * @param   key  keycode of the pressed key
 * @note    ::NOSCFG_FEATURE_CONIN must be defined to 1 
 *          to have this function compiled in.<br>
 *          The alternative to the use of this function is to use
 *          software interrupt 0 to feed keyboard data into the nano layer.
 * @sa      nosKeyGet, nosKeyPressed, NOSCFG_CONIO_KEYBUFSIZE
 */
void    c_nos_keyinput(UVAR_t key);

/**
 * Wait and get the code of the next pressed key.
 * This function blocks until the user presses a key on the keyboard
 * and returns the code of the pressed key as result.
 * @return  ASCII-code of the pressed key
 * @note    ::NOSCFG_FEATURE_CONIN must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosKeyPressed, c_nos_keyinput, NOSCFG_CONIO_KEYBUFSIZE
 */
char    nosKeyGet(void);

/**
 * Test if a key was pressed.
 * This function tests if a key code is available in the keyboard buffer.
 * Even if no key is pressed yet, the function will immediately return.
 * @return  TRUE (nonzero) when a key code is available.
 * @note    ::NOSCFG_FEATURE_CONIN must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosKeyGet, c_nos_keyinput, NOSCFG_CONIO_KEYBUFSIZE
 */
UVAR_t  nosKeyPressed(void);

#endif  /* NOSCFG_FEATURE_CONIN */


#if (DOX!=0)
/**
 * Print a character to the console or terminal. This function
 * must be supplied by the architecture port; it is not callable
 * by the user.
 * @param   c  character to print out.
 * @return  This function should return nonzero (=TRUE) when the
 *          character could be printed. If the function could not
 *          print the character (e.g. the FIFO of a serial line
 *          is full), this function should return zero (=FALSE).
 *          The nano layer will try to send the character later again.
 * @note    This function must not do a CR/LF conversion, a CR
 *          must result in a simple carriage return, and a LF
 *          must result in a simple line feed without returning
 *          the carriage.
 * @note    If this function returns FALSE, the platform port
 *          must do a call to ::c_nos_putcharReady when it is
 *          ready again for accepting new characters. When
 *          ::NOSCFG_CONOUT_HANDSHAKE is disabled, the return
 *          value of this function is ignored.
 * @sa      c_nos_putcharReady, c_nos_keyinput
 */
UVAR_t  p_putchar(char c); 
#endif


#if (DOX!=0) || (NOSCFG_CONOUT_HANDSHAKE != 0)
/**
 * This is the optional handshake function for ::p_putchar.
 * The handshake function is usefull for console output over
 * a serial line. The function ::p_putchar will fail when the
 * transmitter FIFO is full, and the nano layer will save the
 * last character for later transmission. The platform port
 * should than call ::c_nos_putcharReady when the transmitter
 * FIFO has space again (when the last character has left the
 * output shift register); most commonly this is signalled by
 * a hardware interrupt, that would be used to call this
 * handshake function. @n
 * The purpose of this handshaking is to reduce CPU usage
 * by avoiding polling on the standard output until the stdout
 * is ready again. In the current implementation this function
 * triggers a semaphore that wakes the task waiting for
 * service on standard out. @n
 * Note that this function must be supplied by the
 * architecture port; it is not callable by the user.
 * @note To enable this handshake function, the define
 *       ::NOSCFG_CONOUT_HANDSHAKE must be set to 1.
 */
void    c_nos_putcharReady(void);
#endif


#if (DOX!=0) || (NOSCFG_FEATURE_CONOUT != 0)

/**
 * Print a character to the console or terminal.
 * This function prints a single character to the console.
 * No CR/LF conversion is performed.
 * @note    ::NOSCFG_FEATURE_CONOUT must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosPrint, p_putchar
 */
void    nosPrintChar(char c);

/**
 * Print a character string to the console or terminal.
 * This function prints a string of characters (text) to the console.
 * A CR/LF conversion is performed, CR is preceding each LF.
 * @param   s  pointer to zero terminated ASCII string
 * @note    ::NOSCFG_FEATURE_CONOUT must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosPrintChar, p_putchar
 */
void    nosPrint(const char *s);

#endif

#if NOSCFG_FEATURE_CONOUT == 0
#if NOSCFG_FEATURE_PRINTF != 0
#undef  NOSCFG_FEATURE_PRINTF
#define NOSCFG_FEATURE_PRINTF  0
#endif
#endif

#if (NOSCFG_FEATURE_PRINTF != 0) || (NOSCFG_FEATURE_SPRINTF != 0)
typedef void* NOSARG_t;
#endif


#if (DOX!=0) || ((NOSCFG_FEATURE_CONOUT != 0)&&(NOSCFG_FEATURE_PRINTF != 0))

void n_printFormattedN(const char *fmt, NOSARG_t args);

#if (DOX!=0)
/**
 * Print a formated character string to the console or terminal.
 * This function acts like the usual printf function, except that
 * it is limmited to the basic formats. The largest integer that
 * can be displayed is of type INT_t.
 * @param   fmt  format string
 * @param   a1   first argument
 * @note    ::NOSCFG_FEATURE_CONOUT and ::NOSCFG_FEATURE_PRINTF 
 *          must be defined to 1
 *          to have this function compiled in.<br>
 *          This function is not variadic. To print strings with
 *          more than one argument, you may use the functions
 *          nosPrintf2 (2 arguments) to nosPrintf6 (6 arguments).
 * @sa      nosPrintChar, nosPrint
 */
void nosPrintf1(const char *fmt, arg a1);

#else /* DOX!=0 */
#define nosPrintf1(fmt, a1)  \
  do { \
    NOSARG_t args[1]; args[0] = (NOSARG_t)(a1); \
    n_printFormattedN(fmt, args); \
  } while(0);

#define nosPrintf2(fmt, a1, a2)  \
  do { \
    NOSARG_t args[2]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); \
    n_printFormattedN(fmt, args); \
  } while(0);

#define nosPrintf3(fmt, a1, a2, a3)  \
  do { \
    NOSARG_t args[3]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); args[2] = (NOSARG_t)(a3); \
    n_printFormattedN(fmt, args); \
  } while(0);

#define nosPrintf4(fmt, a1, a2, a3, a4)  \
  do { \
    NOSARG_t args[4]; \
    args[0] = (NOSARG_t)(a1); args[1] = (NOSARG_t)(a2); \
    args[2] = (NOSARG_t)(a3); args[3] = (NOSARG_t)(a4); \
    n_printFormattedN(fmt, args); \
  } while(0);

#define nosPrintf5(fmt, a1, a2, a3, a4, a5)  \
  do { \
    NOSARG_t args[5]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); args[2] = (NOSARG_t)(a3); \
    args[3] = (NOSARG_t)(a4); args[4] = (NOSARG_t)(a5); \
    n_printFormattedN(fmt, args); \
  } while(0);

#define nosPrintf6(fmt, a1, a2, a3, a4, a5, a6)  \
  do { \
    NOSARG_t args[6]; \
    args[0] = (NOSARG_t)(a1); args[1] = (NOSARG_t)(a2); \
    args[2] = (NOSARG_t)(a3); args[3] = (NOSARG_t)(a4); \
    args[4] = (NOSARG_t)(a5); args[5] = (NOSARG_t)(a6); \
    n_printFormattedN(fmt, args); \
  } while(0);

#endif /* DOX!=0 */
#endif /* NOSCFG_FEATURE_PRINTF */


#if (DOX!=0) || (NOSCFG_FEATURE_SPRINTF != 0)
#if (DOX!=0)
/**
 * Print a formated character string to a string buffer.
 * This function acts like the usual sprintf function, except that
 * it is limmited to the basic formats. The largest integer that
 * can be displayed is of type INT_t.
 * @param   buf  destination string buffer
 * @param   fmt  format string
 * @param   a1   first argument
 * @note    ::NOSCFG_FEATURE_SPRINTF must be defined to 1 
 *          to have this function compiled in.<br>
 *          This function is not variadic. To print strings with
 *          more than one argument, you may use the functions
 *          nosSPrintf2 (2 arguments) to nosSPrintf6 (6 arguments).
 * @sa      nosPrintf1, nosPrint
 */
void nosSPrintf1(char *buf, const char *fmt, arg a1);

#else /* DOX!=0 */

void n_sprintFormattedN(char *buf, const char *fmt, NOSARG_t args);

#define nosSPrintf1(buf, fmt, a1)  \
  do { \
    NOSARG_t args[1]; args[0] = (NOSARG_t)(a1); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#define nosSPrintf2(buf, fmt, a1, a2)  \
  do { \
    NOSARG_t args[2]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#define nosSPrintf3(buf, fmt, a1, a2, a3)  \
  do { \
    NOSARG_t args[3]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); args[2] = (NOSARG_t)(a3); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#define nosSPrintf4(buf, fmt, a1, a2, a3, a4)  \
  do { \
    NOSARG_t args[4]; \
    args[0] = (NOSARG_t)(a1); args[1] = (NOSARG_t)(a2); \
    args[2] = (NOSARG_t)(a3); args[3] = (NOSARG_t)(a4); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#define nosSPrintf5(buf, fmt, a1, a2, a3, a4, a5)  \
  do { \
    NOSARG_t args[5]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); args[2] = (NOSARG_t)(a3); \
    args[3] = (NOSARG_t)(a4); args[4] = (NOSARG_t)(a5); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#define nosSPrintf6(buf, fmt, a1, a2, a3, a4, a5, a6)  \
  do { \
    NOSARG_t args[6]; \
    args[0] = (NOSARG_t)(a1); args[1] = (NOSARG_t)(a2); \
    args[2] = (NOSARG_t)(a3); args[3] = (NOSARG_t)(a4); \
    args[4] = (NOSARG_t)(a5); args[5] = (NOSARG_t)(a6); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#endif /* DOX!=0 */
#endif /* NOSCFG_FEATURE_SPRINTF */
/** @} */



/*---------------------------------------------------------------------------
 *  BOTTOM HALFS
 *-------------------------------------------------------------------------*/

/** @defgroup bhalf Bottom Halfs
 * @ingroup userapin
 * 
 * <b> Note: This API is part of the nano layer </b>
 *
 * Interrupt service routines can be divided into to halfs: the top and
 * the bottom half. The top half is that piece of code, that is directly
 * executed when the processor gets an hardware interrupt signalled.
 * This code is usually executed with globally disabled interrupts. Thus,
 * a second interrupt that happens while the first interrupt is still
 * serviced, will be delayed until the processor has left the currently
 * running ISR again. To minimize interrupt delay, only the critical part
 * of the ISR (the top half) is executed at interrupt level, all non
 * critical code is executed at task level (bottom half). Because the bottom
 * half is interruptable, critical interrupts won't be delayed too much.
 * @{
 */

#if (DOX!=0) || (NOSCFG_FEATURE_BOTTOMHALF != 0)
/**
 * Bottom half function. Registers a new bottom half.
 * @param   number      Number of the bottom half. Must be between
 *                      0 and ::NOS_MAX_BOTTOMHALFS - 1.
 * @param   func        pointer to the bottom half function that shall be
 *                      executed when the bottom half is triggered.
 * @param   arg         optional argument passed to function func.
 * @return  Zero on success. Nonzero values denote an error.
 * @note    To unregister a bottom half function again, you may call
 *          this function with funcptr = NULL, or alternatively,
 *          you can use the macro ::nosBottomHalfUnregister. <br>
 *          ::NOSCFG_FEATURE_BOTTOMHALF must be defined to 1
 *          to enable bottom half support.
 * @note    Important! A bottom half function is not allowed to block,
 *          that means such a function must not call functions that
 *          may block (for example, this functions are posTaskSleep,
 *          posSemaGet, posSemaWait, posMutexLock).
 * @sa      nosBottomHalfUnregister, nosBottomHalfStart
 */
VAR_t nosBottomHalfRegister(UVAR_t number, NOSBHFUNC_t func, void *arg);

/**
 * Bottom half function. Unregisters a bottom half.
 * @param   number      Number of the bottom half to unregister.
 *                      Must be between 0 and ::NOS_MAX_BOTTOMHALFS - 1.
 * @return  Zero on success. Nonzero values denote an error.
 * @note    ::NOSCFG_FEATURE_BOTTOMHALF must be defined to 1
 *          to enable bottom half support.
 * @sa      nosBottomHalfRegister
 */
#define nosBottomHalfUnregister(number) \
          nosBottomHalfRegister(number, NULL, NULL)

/**
 * Bottom half function. Triggers a bottom half function.
 * The bottom half is executed when the interrupt level is left.
 * @param   number      Number of the bottom half. Must be between
 *                      0 and ::NOS_MAX_BOTTOMHALFS - 1.
 * @note    This function is called by the top half of an
 *          interrupt service routine. Note that the ISR must be
 *          enclosured by a ::c_pos_intEnter / ::c_pos_intExit pair. <br>
 *          ::NOSCFG_FEATURE_BOTTOMHALF must be defined to 1
 *          to enable bottom half support.
 * @sa      nosBottomHalfRegister, nosBottomHalfUnregister
 */
void nosBottomHalfStart(UVAR_t number);

#endif
/** @} */



/*---------------------------------------------------------------------------
 *  CPU USAGE
 *-------------------------------------------------------------------------*/

#if (DOX!=0) || (NOS_FEATURE_CPUUSAGE != 0)
/** @defgroup cpuusage CPU Usage Calculation
 * @ingroup userapin
 * The nano layer features CPU usage measurement. If this feature is
 * enabled, the system start is delayed for approximately one second that
 * is needed to calibrate the idle loop counter.
 * The CPU usage statistics is updated one time per second.
 * @{
 */
/**
 * Calculate and return the percentage of CPU usage.
 * @return  percentage of CPU usage (0 ... 100 %%)
 * @note    ::NOS_FEATURE_CPUUSAGE must be defined to 1
 *          to have this function compiled in.
 */
UVAR_t nosCpuUsage(void);
#endif
/** @} */



/*---------------------------------------------------------------------------
 *  ABSTRACTED FUNCTIONS
 *-------------------------------------------------------------------------*/

/** @defgroup absfunc Abstracted Functions
 * @ingroup userapin
 * @{
 */

#if (DOX!=0) || (NOSCFG_FEATURE_TASKCREATE != 0)
/**
 * Generic task function. Creates a new task.
 * @param   funcptr     pointer to the function that shall be executed
 *                      by the new task.
 * @param   funcarg     optional argument passed to function funcptr.
 * @param   priority    task priority. Must be in the range
 *                      0 .. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                      The higher the number, the higher the priority.
 * @param   stacksize   Size of the stack memory. If set to zero,
 *                      a default stack size is assumed
 *                      (see define ::NOSCFG_DEFAULT_STACKSIZE).
 * @param   name        Unique name for the task. This parameter is yet
 *                      ignored and can be set to NULL (=unnamed task).
 * @return  handle to the task. NULL is returned when the
 *          task could not be created.
 * @note    ::NOSCFG_FEATURE_TASKCREATE must be defined to 1
 *          to have this function compiled in.
 * @sa      posTaskExit
 */
POSTASK_t nosTaskCreate(POSTASKFUNC_t funcptr, void *funcarg,
                        VAR_t priority, UINT_t stacksize,
                        const char* name);
#endif
/** @} */



/*---------------------------------------------------------------------------
 *  INITIALIZATION
 *-------------------------------------------------------------------------*/

/**
 * Operating System Initialization.
 * This function initializes the operating system and starts the
 * first tasks: The idle task and the first user task.
 * @param   firstfunc   pointer to the first task function that
 *                      will run in the multitasking environment.
 * @param   funcarg     optional argument passed to the first task.
 * @param   priority    task priority. Must be in the range
 *                      0 .. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                      The higher the number, the higher the priority.
 * @param   taskStackSize  size of the stack memory for the first task.
 * @param   idleStackSize  size of the stack memory for the idle task.
 * @note    This function replaces the function ::posInit if the nano
 *          layer is enabled and linked to the destination application.
 */
void  nosInit(POSTASKFUNC_t firstfunc, void *funcarg, VAR_t priority,
              UINT_t taskStackSize, UINT_t idleStackSize);

#ifndef _N_CORE_C
#ifndef _POSCORE_C
#define posInit _Please_use_nosInit_instead_of_posInit_
#endif
#endif


#endif /* _POS_NANO_H */
