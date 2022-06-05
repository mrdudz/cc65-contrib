/* fixed point library examples */
/* (C) Steffen Goerzig          */

/* EXIT_SUCCESS */
#include <stdlib.h>
/* clrscr(), cprintf() */
#include <conio.h>
/* fixed and double fixed */
#include "fixedpoint.h"

int main (void){
  fixed fVar, fVar2, fResult;
  doublefixed dfVar, dfVar2, dfResult;
  char str[33]="";

  clrscr();
  cprintf("Examples for fixed and doublefixed\n\r");
  cprintf("\n\r");

  /* 1.1 fixed point */
  floatStringToFixed("1,99609375", fVar);
  unsignedCharToFixed(3, fVar2);
  fixedMul(fVar, fVar2, fResult);
  fixedToFloatString(fResult, str);
  cprintf("fixed result: \n\r");
  cprintf("%s\n\r", str);
  cprintf("\n\r");

  /* 3.1 double fixed point */
  doublefixedSetNumberBytesAfterPoint(1);
  fixedToDoublefixed(fVar, dfVar);
  unsignedLongToDoublefixed(16777214, dfVar2);
  doublefixedAdd(dfVar, dfVar2, dfResult);
  doublefixedToBitString(dfResult, str);
  cprintf("doublefixed 3.1 result:\n\r");
  cprintf("%s\n\r", str);
  cprintf("\n\r");

  /* 2.2 double fixed point */
  doublefixedSetNumberBytesAfterPoint(2);
  /* convert from 3.1 to 2.2. double fixed point */
  doublefixed31To22(dfResult, dfVar);
  unsignedIntToDoublefixed(65535, dfVar2);
  doublefixedSub(dfVar, dfVar2, dfResult);
  doublefixedToBitString(dfResult, str);
  cprintf("doublefixed 2.2 result:\n\r");
  cprintf("%s\n\r", str);
  cprintf("\n\r");

  /* 1.3 double fixed point */
  doublefixedSetNumberBytesAfterPoint(3);
  bitStringToDoublefixed("11111111111111111111111111111111", dfVar);
  unsignedCharToDoublefixed(255, dfVar2);
  doublefixedSub(dfVar, dfVar2, dfResult);
  doublefixedToBitString(dfResult, str);
  cprintf("doublefixed 1.3 result:\n\r");
  cprintf("%s\n\r", str);
  cprintf("\n\r");

  /* back to 1.1 fixed point */
  doublefixedToFixed(dfResult, fResult);
  fixedToFloatString(fResult, str);
  cprintf("doublefixed to fixed result:\n\r");
  cprintf("%s\n\r", str);
  cprintf("\n\r");

  return EXIT_SUCCESS;
}
