;*****************************************************************************;
;*                                                                           *;
;*				     cbm.h			       	     *;
;*                                                                           *;
;*		        CBM system specific definitions			     *;
;*                                                                           *;
;*                                                                           *;
;*                                                                           *;
;* (C) 1998-2002 Ullrich von Bassewitz                                       *;
;*               Wacholderweg 14                                             *;
;*               D-70597 Stuttgart                                           *;
;* EMail:        uz@musoftware.de                                            *;
;*                                                                           *;
;*                                                                           *;
;* This software is provided 'as-is', without any expressed or implied       *;
;* warranty.  In no event will the authors be held liable for any damages    *;
;* arising from the use of this software.                                    *;
;*                                                                           *;
;* Permission is granted to anyone to use this software for any purpose,     *;
;* including commercial applications, and to alter it and redistribute it    *;
;* freely, subject to the following restrictions:                            *;
;*                                                                           *;
;* 1. The origin of this software must not be misrepresented; you must not   *;
;*    claim that you wrote the original software. If you use this software   *;
;*    in a product, an acknowledgment in the product documentation would be  *;
;*    appreciated but is not required.                                       *;
;* 2. Altered source versions must be plainly marked as such, and must not   *;
;*    be misrepresented as being the original software.                      *;
;* 3. This notice may not be removed or altered from any source              *;
;*    distribution.                                                          *;
;*                                                                           *;
;*****************************************************************************;

;*****************************************************************************;
;*                                 Variables                                 *;
;*****************************************************************************;



;* The file stream implementation and the POSIX I;O functions will use the
;* following variables to determine the file type and the disk unit to use.
;*;
.DEFINE _curunit $ba	;          ;* Default 8 *;


;*****************************************************************************;
;*                      Characters codes (CBM charset)                       *;
;*****************************************************************************;



.DEFINE CH_HLINE    		 96
.DEFINE CH_VLINE    		125
.DEFINE	CH_ULCORNER 		176
.DEFINE CH_URCORNER 		174
.DEFINE CH_LLCORNER 		173
.DEFINE CH_LRCORNER 		189
.DEFINE CH_TTEE	    		178
.DEFINE CH_RTEE	    		179
.DEFINE CH_BTEE	    		177
.DEFINE CH_LTEE	    		171
.DEFINE CH_CROSS    		123
.DEFINE CH_CURS_UP		145
.DEFINE CH_CURS_DOWN		 17
.DEFINE CH_CURS_LEFT		157
.DEFINE CH_CURS_RIGHT		 29
.DEFINE CH_PI			126
.DEFINE CH_DEL			 20
.DEFINE CH_INS			148
.DEFINE CH_ENTER                13
.DEFINE CH_STOP                   3
.DEFINE CH_ESC                   27
.DEFINE CH_UNDERLINE		164
.DEFINE CH_HOME 19
.DEFINE CH_CLRHOME 147

.DEFINE CH_REVON		18
.DEFINE CH_REVOFF		146
.DEFINE CH_BLACK	144
.DEFINE CH_WHITE	5
.DEFINE CH_RED		28
.DEFINE CH_CYAN	159
.DEFINE CH_VIOLET 156
.DEFINE CH_GREEN 30
.DEFINE CH_BLUE 31
.DEFINE CH_YELLOW 158
.DEFINE CH_ORANGE 129
.DEFINE CH_BROWN 149
.DEFINE CH_LIGHTRED 150
.DEFINE CH_GRAY1 151
.DEFINE CH_GRAY2 152
.DEFINE CH_LIGHTGREEN 153
.DEFINE CH_LIGHTBLUE 154
.DEFINE CH_GRAY3 155

