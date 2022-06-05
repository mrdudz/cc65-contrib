#include "sqrt.h"
#include <stdio.h>
#include <conio.h>
#include <assert.h>
#include <stdlib.h>

#define ERRFUNC(a) (2*(a)+1)
/*Difference between the a^2=sqrt16(b*b)^2 and 
	b^2 = sqrt(b*b)^2, which is the real root, have to
	be lower than the value calculated by upper formula.
	The formula is derived by setting a=trunc(b), which is
	minimum b-1. Then the difference (a+1)^2-a*a is calculated.
*/

void main(void){
unsigned char i=0;
unsigned int i2=0;
unsigned int temp;

	for(i=0;i<255;i++){
		temp=sqrt8(i);
		printf("%hu: %hu ** ",(unsigned char)i,(unsigned char)temp);
	  assert(abs(temp*temp-i)<=ERRFUNC(temp));
		/*assert, that the root is between the error-marks*/
	}
	
	cgetc();

	for(i2=0;i2<0xffff;i2++){
		temp=sqrt16(i2);
		if(i2%100==0){printf("%hu: %hu ** ",i2,temp);}

		if(abs(temp*temp-i2)>ERRFUNC(temp)){
			printf("Sqrt wrong! b:%u, trunc(sqrt(b)):%u",i2,temp);
			abort();
		}/*assert, that the root is between the error-marks*/
	}
}
