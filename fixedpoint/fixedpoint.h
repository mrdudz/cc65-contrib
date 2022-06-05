/******************************************/
/* (C) Steffen Goerzig                    */
/* 1.1 fixed point                        */
/* 1.3 and 2.2 and 3.1 double fixed point */
/******************************************/

#ifndef _FIXEDPOINT_H
#define _FIXEDPOINT_H

/* umul16x16r32(), udiv32by16r16() */
#include <cc65.h>

/* 1.1 fixed point type */
typedef unsigned int fixed;

/* byte/bit size for internal use */
#define fixedNumberBytesAfterPoint 1
#define fixedNumberBitsAfterPoint fixedNumberBytesAfterPoint * 8
#define fixedZero 0

fixed internalFloatStringToFixed(char* value);
#define floatStringToFixed(value, result) {   \
  result = internalFloatStringToFixed(value); \
}

void internalFixedToFloatString(fixed value, char* result);
#define fixedToFloatString(value, result) {  \
  internalFixedToFloatString(value, result); \
}

#define bitStringToFixed(value, result) {  \
   result = strtoul (value, NULL, 2);      \
}

#define fixedToBitString(value, result) { \
  utoa(value, result, 2);                 \
}

#define fixedToUnsignedInt(value, result){       \
  result = (value >> fixedNumberBitsAfterPoint); \
}

#define fixedToUnsignedChar(value, result){                     \
  result = (unsigned char)(value >> fixedNumberBitsAfterPoint); \
}

#define unsignedIntToFixed(value, result){       \
    result = ((fixed)value << fixedNumberBitsAfterPoint);	\
}

#define unsignedCharToFixed(value, result){             \
  result = ((fixed)value << fixedNumberBitsAfterPoint); \
}

/* result = op1+op2 */
#define fixedAdd(op1, op2, result){ \
  result = (op1+op2);               \
}

/* result = op1-op2 */
#define fixedSub(op1, op2, result){ \
  result = (op1-op2);               \
}

/* result = op1*op2 */
#define fixedMul(op1, op2, result){		              	               \
  result = (fixed)(umul16x16r32 (op1, op2) >> fixedNumberBitsAfterPoint); \
}
 
/* result = op1/op2 */
#define fixedDiv(op1, op2, result){                       \
  unsigned long o1;                                       \
  if (op2 == 0){					  \
    /* division by zero */				  \
    result = fixedZero;					  \
  }							  \
  else {						  \
    o1 = (unsigned long)op1 << fixedNumberBitsAfterPoint; \
    result = (fixed)udiv32by16r16(o1, op2);	  \
  }                                                       \
}

/* result = op1*op2 */
fixed internalFixedMulUnsignedChar(fixed op1, unsigned char op2);
#define fixedMulUnsignedChar(op1, op2, result) {   \
  result = internalFixedMulUnsignedChar(op1, op2); \
}

/*
Possible fixed point byte combinations:
before.after
     1.3
     2.2
     3.1
total fixed bytes (before point and after point): 4
*/

/* 4 byte fixed point type */
typedef unsigned long doublefixed;

/* byte/bit size for internal use */
extern unsigned char doublefixedNumberBitsAfterPoint;

/* numBytes must be in range {1,2,3}, default: 2 */
#define doublefixedSetNumberBytesAfterPoint(numBytes) {                             \
  doublefixedNumberBitsAfterPoint = (unsigned int)numBytes * 8;	                    \
  if((doublefixedNumberBitsAfterPoint<8) || (doublefixedNumberBitsAfterPoint>24)) { \
    doublefixedNumberBitsAfterPoint = (unsigned int)16;                             \
  }                                                                                 \
}

#define fixedToDoublefixed(value, result) {                                                     \
  result = (doublefixed)value << (doublefixedNumberBitsAfterPoint - fixedNumberBitsAfterPoint); \
}

#define doublefixedToFixed(value, result) {                                                     \
  result = (doublefixed)value >> (doublefixedNumberBitsAfterPoint - fixedNumberBitsAfterPoint); \
}

#define bitStringToDoublefixed(value, result) { \
   result = strtoul (value, NULL, 2);           \
}

#define doublefixedToBitString(value, result) { \
  ultoa(value, result, 2);                      \
}

#define doublefixedToUnsignedInt(value, result){             \
  result = (unsigned int)(value >> doublefixedNumberBitsAfterPoint);	\
}

#define doublefixedToUnsignedLong(value, result){             \
  result = (unsigned long)(value >> doublefixedNumberBitsAfterPoint);	\
}

#define doublefixedToUnsignedChar(value, result){                     \
  result = (unsigned char)(value >> doublefixedNumberBitsAfterPoint); \
}

#define unsignedLongToDoublefixed(value, result){        \
  result = ((doublefixed)value << doublefixedNumberBitsAfterPoint);	\
}

#define unsignedIntToDoublefixed(value, result){        \
  result = ((doublefixed)value << doublefixedNumberBitsAfterPoint);	\
}

#define unsignedCharToDoublefixed(value, result){                    \
  result = ((doublefixed)value << doublefixedNumberBitsAfterPoint); \
}

#define doublefixed13To22(value, result) { \
  result = (doublefixed)value >> 8 ;       \
}

#define doublefixed13To31(value, result) { \
  result = (doublefixed)value >> 16 ;      \
}

#define doublefixed22To13(value, result) { \
  result = (doublefixed)value << 8;        \
}

#define doublefixed22To31(value, result) { \
  result = (doublefixed)value >> 8;        \
}

#define doublefixed31To22(value, result) { \
  result = (doublefixed)value << 8;        \
}

#define doublefixed31To13(value, result) { \
  result = (doublefixed)value << 16;       \
}

/* result = op1+op2 */
#define doublefixedAdd(op1, op2, result){ \
  result = (op1+op2);             \
}

/* result = op1-op2 */
#define doublefixedSub(op1, op2, result){ \
  result = (op1-op2);             \
}

// biggest cc65 data type has 4 bytes,
// so doublefixed div and mul do not work properly
/* result = op1*op2 */
//#define doublefixedMul(op1, op2, result){				\
//  result = (doublefixed)((op1*op2) >> doublefixedNumberBitsAfterPoint); \
//}
/* result = op1/op2 */
//#define doublefixedDiv(op1, op2, result){	      \
//  doublefixed o1;                                     \
//  if (op2 == 0){					\
//    /* division by zero */				\
//    result = (doublefixed)fixedZero;			\
//  }							\
//  else {						      \
//    o1 = (doublefixed)op1 << doublefixedNumberBitsAfterPoint; \
//    result = (doublefixed)(o1 / op2);				\
//  }								\
//}


#endif /* _FIXEDPOINT_H */
