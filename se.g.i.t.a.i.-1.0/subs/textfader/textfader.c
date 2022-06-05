#include <stdbool.h>
#include <textfader.h>

bool textfadeok = false;

void textfader(void){
	static signed char fadepos = 0;
	static signed char fadedirection = 1;
	static unsigned char fade12count = 0;
	
	if (textfadeok){
		
		(fade12count&2)?
			fadecode2(fadepos):
			fadecode1(fadepos);
	
		fadepos+=fadedirection;
		if( (fadepos>64)||(fadepos<0) ){
			fadedirection*=-1;
			textfadeok = false;
			++fade12count;
		}
	}
}

bool textfades(void){
	return textfadeok;
}

void allow_text_fade(void){
	textfadeok = true;
}	
