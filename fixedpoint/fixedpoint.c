/***********************/
/* (C) Steffen Goerzig */
/* fixed point:        */
/* 1.1 fixed point     */
/* double fixed point: */
/* 1.3 fixed point     */
/* 2.2 fixed point     */
/* 3.1 fixed point     */
/***********************/

/* strtoul(), ultoa() */
#include <stdlib.h>
/* strtok(), strlen(), strncpy() */
#include <string.h>
#include "fixedpoint.h"

/* when the preprocessor can handle float, try this:
#define floatToFixed(value) (fixed)(value * (1<<fixedNumberBitsAfterPoint))
*/
      
fixed internalFloatStringToFixed(char* value) {
  unsigned int stringLength;
  unsigned char i;					      
  unsigned long tmpVal;                                               
  char* stringPart;                                                   
  char delimiter[] = ",.";                                            
  char stringCopy[13];     					      
  fixed result;
  unsigned char fixedRoundUp;                           		      
  strncpy(stringCopy, value, 13);                             
  stringCopy[12] = '\0';  					      
  stringPart = strtok(stringCopy, delimiter);                         
  result = strtoul (stringPart, NULL, 10) * (1ul<<fixedNumberBitsAfterPoint); 
  stringPart = strtok(NULL, delimiter);                               
  stringLength = strlen(stringPart);                                  
  if (stringLength>7) {                                               
    stringLength=7;
    if(stringPart[7]-(unsigned char)'0'>4){
      /* ensure that .99609375 will be 11111111
	 and not 11111110 */
      if(stringPart[6]-(unsigned char)'0'<9){
	stringPart[6]=stringPart[6]+1;
      }
    }
    stringPart[7] = '\0';
  }					      
  tmpVal = strtoul (stringPart, NULL, 10) * (1ul<<fixedNumberBitsAfterPoint); 
  for(i=0; i<(unsigned char)stringLength; i++) {                                     
    if((tmpVal % 10)>4){                                              
      fixedRoundUp = 1;						      
    }                                                                 
    else {                                                            
      fixedRoundUp = 0;						      
    }                                                                 
    tmpVal = tmpVal / 10;                                             
  }                                                                   
  result = result + tmpVal;
  // find a 1.1 value closer to the original float value
  if (fixedRoundUp > 0) {                                                 
    result = result + 1;   					      
  }        
  return result;
}

#define internalFixedTmpStringLength 9

void internalFixedToFloatString(fixed value, char* result) {
  unsigned long copyValue;                                               
  unsigned long tmpVar;                                               
  unsigned long tmpFactorBase10=10;                                    
  unsigned long tmpFactorBase2=2;                                     
  unsigned char found=1;                                              
  unsigned char diff;                           			      
  unsigned char i;                           			      
  char tmpString[internalFixedTmpStringLength];
  char tmpString2[internalFixedTmpStringLength];
  unsigned char minusOne=0;

  copyValue = (unsigned long)value;
  tmpVar = (copyValue >> fixedNumberBitsAfterPoint);                            
  ultoa(tmpVar, result, 10);                                          
  tmpVar = strlen(result);                                            
  result[tmpVar] = '.';                                               
  result[tmpVar+1] = '\0';                                            
  tmpVar = (copyValue >> fixedNumberBitsAfterPoint);                            
  tmpVar = (tmpVar << fixedNumberBitsAfterPoint);                           
  tmpVar = (copyValue - tmpVar);                             	      
  ultoa(tmpVar, tmpString2, 2);                                       
  diff = (unsigned char)(fixedNumberBitsAfterPoint-strlen(tmpString2));
  for(i=0;i<diff;i++){                                                
    tmpString[i] = '0';                                               
  }                                                                   
  tmpString[diff] = '\0';	    	                              
  strncat(tmpString,tmpString2,internalFixedTmpStringLength-strlen(tmpString));     	      
  tmpVar = 0;
  if (tmpString[0]=='1'){                                           
    tmpVar = tmpVar + (tmpFactorBase10 / tmpFactorBase2);           
  }                                                                 
  tmpVar = tmpVar * 10;                                             
  if (tmpVar==0){
    tmpVar = 10;                              			      
    minusOne=1;
  }
  for (i=1;i<fixedNumberBitsAfterPoint;i++){                                
    tmpFactorBase10 = tmpFactorBase10 * 10;                           
    tmpFactorBase2 = tmpFactorBase2 * 2;                              
    if (tmpString[i]=='1'){                                           
      tmpVar = tmpVar + (tmpFactorBase10 / tmpFactorBase2);           
    }                                                                 
    tmpVar = tmpVar * 10;                                             
  }                                                                   
  ultoa(tmpVar, tmpString, 10);                                       
  if(minusOne>0){
    tmpString[0] = tmpString[0]-1;                                      
  }
  strncat(result,tmpString,internalFixedTmpStringLength);                                       
  if(strlen(result)>0) {                                              
    tmpVar = strlen(result);                                          
    while(found==1) {                                                 
      tmpVar = tmpVar - 1;                                            
      if(result[tmpVar]=='0' && (result[tmpVar-1]!='.')) {	      
	result[tmpVar] = '\0';
      }                                       
      else {  
	found = 0; 
      }                                            
    }                                                                
  }                                                                  
}

/* op1: type fixed point 
   op2: type uint (byte) */
fixed internalFixedMulUnsignedChar(fixed op1, unsigned char op2){
  fixed r, z;
  r = fixedZero;
  if(op2>=128){
    r = op1 << 7;
    op2 = op2 - 128;
  }
  if(op2>=64){
    z = op1 << 6;
    fixedAdd(r, z, r);
    op2 = op2 - 64;
  }
  if(op2>=32){
    z = op1 << 5;
    fixedAdd(r, z, r);
    op2 = op2 - 32;
  }
  if(op2>=16){
    z = op1 << 4;
    fixedAdd(r, z, r);
    op2 = op2 - 16;
  }
  if(op2>=8){
    z = op1 << 3;
    fixedAdd(r, z, r);
    op2 = op2 - 8;
  }
  if(op2>=4){
    z = op1 << 2;
    fixedAdd(r, z, r);
    op2 = op2 - 4;
  }
  if(op2>=2){
    z = op1 << 1;
    fixedAdd(r, z, r);
    op2 = op2 - 2;
  }
  if(op2==1){
    fixedAdd(r, op1, r);
  }
  return r;
}

/* default is 2.2 fixed point == 16 bits after point */
unsigned char doublefixedNumberBitsAfterPoint=16;
