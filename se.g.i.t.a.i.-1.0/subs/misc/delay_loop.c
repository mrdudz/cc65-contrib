#include <time.h>

void delay_loop(int delay){
	long int curclock;
	
	curclock = clock();
	while( (clock()-curclock) < delay);
}
