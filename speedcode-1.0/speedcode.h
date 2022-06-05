#ifndef SPDC_H
#define SPDC_H

#include <stdbool.h>

#define RTS 0x60

typedef struct 
{ void* speedcodepart;
	unsigned char size;
}SPEEDCODE_SU;

unsigned char* create_speedcode(SPEEDCODE_SU* spdcdef, void(*dest)(void), bool(*func)(unsigned int reppos) );