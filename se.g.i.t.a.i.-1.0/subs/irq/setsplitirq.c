#include <stdbool.h>

#include <logo.h>
#include <irq.h>

void fastcall setsplitirq(bool logopos, void* irqadr){
	switch_raster_irq(logopos?50+(LOGOHEIGHT*8):50+((LOGOY2)*8)-4,irqadr);
}
