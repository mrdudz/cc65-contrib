#ifndef __MISC_H
#define __MISC_H

#define SCRW 40

void delay_loop(int delay);
bool getspacestat(void);
void fastcall savezp(unsigned char* adr);
void fastcall restorezp(unsigned char* adr);

#endif