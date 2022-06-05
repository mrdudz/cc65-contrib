#ifndef _IRQ_H
#define _IRQ_H
	#include <stdbool.h>
	
	#define IRQ_VECTOR ((void**)0x314)
	#define NMI_VECTOR ((void**)0x318)
	void irqinit(void);
	void fastcall switch_raster_irq(unsigned int line, void* adr);
	void rasterirq(void);
	void fastcall setsplitirq(bool logopos, void* irqadr);
	void stop_irq(void);
	
#endif