#include <stdbool.h>
#include <intro.h>

/* Flag for music-fadeout. True if music is allowed to fade off, false if it is not.*/
bool music_fade=false;
int loudness = 255;
	
void music_fader(void){	
	if(music_fade){
		if(loudness>=(256/PARABELTABLEN))
			{loudness -= (256/PARABELTABLEN);}
		else{loudness =0;}
		*MUSIC_VOLUME = loudness >> 4;
	}
}	
	
void allow_music_fade(void){
	music_fade = true;
}

void wait_music_fade(void){
	while(loudness!=0);
}
