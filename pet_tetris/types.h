/*****************************************************************************
* TETRIS for Commodore PET (using cc65 compiler)
* (c) Tim Howe, March 2010
*
* Types, structures,  defines and common macros
*****************************************************************************/

#ifndef _TYPES_H_

    #ifndef TRUE
        #define TRUE    0xFF
        #define FALSE   0
    #endif

    #ifdef __CC65__  // Detect cc65 compiler
        typedef signed char        INT8, * pINT8;
        typedef unsigned char     UINT8, * pUINT8;
        typedef signed int        INT16, * pINT16;
        typedef unsigned int     UINT16, * pUINT16;
        typedef signed long       INT32, * pINT32;
        typedef unsigned long    UINT32, * pUINT32;
        typedef void               VOID, * pVOID;
        typedef unsigned char      BOOL, * pBOOL;

        #define _LITTLE_ENDIAN_
        #define _MSBYTE_        3
        #define _LSBYTE_        0
    #endif

    #define _TYPES_H_

#endif

/* EOF */


