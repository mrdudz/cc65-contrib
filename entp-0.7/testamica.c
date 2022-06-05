#include "entpack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <conio.h>
#include <cbm.h>
#include <assert.h>
#include <string.h>

#define MAXFNAMELEN 16

#define VICBANK 0xc5
#define VICGFXMODE 0x3b
#define VICGFXMULT 0x18
#define SCRCHARADDR 0x38
#define VICTEXTMODE 0x1b
#define VICGFXHIRES 0x8
#define BASICSCRCHARADDR 0x14
#define VICBASICBANK 0xc7

#define BGBITMAP 0xa000
#define BGSCREEN 0x8c00
#define COMPRESSEDBUFSIZE 0x2800

void filldepackarea(char *area){
	memset(area,AMICA_KENNBYTE*256, COMPRESSEDBUFSIZE);
}

void initgfxmode(void){
	VIC.ctrl1 = VICGFXMODE;
	VIC.ctrl2 = VICGFXMULT;	
  VIC.addr = SCRCHARADDR;
  CIA2.pra = VICBANK;
}

void leavegfxmode(void){
	VIC.ctrl1 = VICTEXTMODE;
	VIC.ctrl2 = VICGFXHIRES;	
  VIC.addr = BASICSCRCHARADDR;
  CIA2.pra = VICBASICBANK;
}

unsigned char _curunit = 8;

void main(void){
	char filename[MAXFNAMELEN] = "???keeper*";
  char *compressed_data = malloc(COMPRESSEDBUFSIZE);
	unsigned bytes_read;
	unsigned char bgcol;

  /*printf("Insert Filename:\n");
  fgets(filename, MAXFNAMELEN, stdin);
  printf("%s\n",filename);
*/

  filldepackarea(compressed_data);
  bytes_read = cbm_load(filename, _curunit, compressed_data);
  assert(AMICA_END != *compressed_data);
  assert(0 != bytes_read);

  
  memset((char*)BGBITMAP,0,E_BITMAPSIZE);
  memset((char*)BGSCREEN,0xbc,E_SCRCOLRAMSIZE);
  memset((char*)COLOR_RAM,0x0f,E_SCRCOLRAMSIZE);
  
  entp_amicapaint(compressed_data, (char*)BGBITMAP, (char*)BGSCREEN,	
  								(char*)COLOR_RAM,& bgcol);
                
  VIC.bgcolor0 = 
  	bgcol;
  initgfxmode();

  while(!kbhit());
  leavegfxmode();
 	free(compressed_data);
}

