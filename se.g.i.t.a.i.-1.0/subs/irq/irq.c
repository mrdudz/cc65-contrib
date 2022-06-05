#include <cbm.h>
#include <irq.h>

void* old_irq_pointer;
extern unsigned char* rti;

void fastcall switch_raster_irq(unsigned int line, void* adr){
	VIC.rasterline = line&255;
	if(line>>8){
		VIC.ctrl1 |= 0x80;
	}else{
		VIC.ctrl1 &= 0x7f;
	}
	
	*IRQ_VECTOR = adr;
}

void irqinit(void){
	VIC.imr = 0x81;
	CIA1.icr = 0x7f;
	asm("bit %w",(int)&CIA1.icr);
	asm("lda %w",(int)&VIC.irr);
	asm("sta %w",(int)&VIC.irr);
	
	*NMI_VECTOR = &rti;
	old_irq_pointer = *IRQ_VECTOR;
}

void stop_irq(void){
	VIC.imr = 0xf0;
	//CIA1.icr = 0x81;
	//must not be enabled, because the c startup 
	//allows the system-irq. This leads to a crash, when
	//the second part is started
	asm("bit %w",(int)&CIA1.icr);
	asm("lda %w",(int)&VIC.irr);
	asm("sta %w",(int)&VIC.irr);
	
	*IRQ_VECTOR = old_irq_pointer;
}
