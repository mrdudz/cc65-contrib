#include <cbm.h>
#include <string.h>
#include <logo.h>
#include <intro.h>

void fillarea(unsigned char* start, unsigned int width,
	unsigned int height, unsigned int add, unsigned char value){
	
	unsigned int ycount;
	
	for(ycount = 0; ycount < height; ycount++){
		memset(start, value, width);
		start += add;
	}
} 
	
void showlogo(void){
	//Initialize Color-Ram for top-logo
	memcpy(COLOR_RAM+(LOGOY1*0x28), logocolors, LOGOHEIGHT*0x28);
	
	//Copy logo from top of screen1 to bottom of screen2
	memcpy(_LOGOSCRN2_LOAD__+(LOGOY2*0x28), _LOGOSCRN1_LOAD__, LOGOHEIGHT*0x28);
	
	//Set screen colors for text area
	memset(_LOGOSCRN1_LOAD__+(LOGOY1+LOGOHEIGHT)*0x28, 0, (25-LOGOHEIGHT)*0x28);
	memset(_LOGOSCRN2_LOAD__, 0, (25-LOGOHEIGHT)*0x28);
	
	//Clear Color-Ram for text-area
	memset(COLOR_RAM+(LOGOY1+LOGOHEIGHT)*0x28, 0, (25-LOGOHEIGHT)*0x28);
	
	//Fill Textbackground with pattern for MC1
	fillarea(_FRAMEBITMAP1_LOAD__+(FRAMEAREA1X*8)+(FRAMEAREA1Y*0x140), FRAMEAREAWIDTH*8, 
					 FRAMEAREAHEIGHT, 0x140, FRAMEAREAFILL);
}
