#ifndef _PARALAX_H
#define _PARALAX_H
	#include <stdbool.h>
	extern unsigned int _MUSIC_LOAD__;
	
	#define SUNAREAW 40
	#define SUNAREAH 20
	
	void paralax(void);
	void init_parscreen(void);
	void* fastcall createspeedcode1(bool fadecode);
	void* fastcall createspeedcode2(void);
	//procedures allocates memory and creates speedcode
	void InitMountains(void);
	void sprinit(void);
	void irqinit(void);
	void __fastcall__ musicinit(unsigned int tune_subtune_nr);
	
	void copyclowds(void);
	void mainloop(void);
	void showlogo(void);
	unsigned char is_sun_fade(void);
	
	#define _assert_(cond) VIC.crtl1=$1b;\\
		VIC.ctrl2=$08;\\
		VIC.addr=$16;\\
		CIA1.pra=$c7;\\
		assert(cond)
	//no semicolon after last command, because it is supplied when macro is called	
	
#endif
