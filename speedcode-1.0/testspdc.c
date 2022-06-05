/* To try this out: Start testspdc.run or compile your own version.
	 Make sure, something is written in th first screenline.
	 Type SYS Destination Adr, Return. 
	 (Replace Destination Adr with the adress of the program output)*/
	 
#include <stdio.h>
#include <speedcode.h>
#include <stdlib.h>
#include <stdbool.h>

bool modspeedcode(unsigned int reppos){
	extern unsigned int srcadr,destadr;
	destadr=srcadr;
	++srcadr;
	return (reppos<40)? true:false;
}

int main(void)
{	extern SPEEDCODE_SU spdcdef;
	void* speedcodeadr = malloc(spdcdef.size*39+1);
	printf("Adress of speedcode %d($%x)\n"
				 "Length of speedcode %d\n"
				 "Number of repeats %d\n"
				 "Destination Adr %d($%x)\n", spdcdef.speedcodepart,spdcdef.speedcodepart, 
				 														spdcdef.size, 39, speedcodeadr,
				 														speedcodeadr);
	create_speedcode(&spdcdef,speedcodeadr,&modspeedcode); 
	free(speedcodeadr);
	return 0;
}
