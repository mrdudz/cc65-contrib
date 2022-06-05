#include <fontdisp.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <intro.h>
#include <misc.h>

#define TIME_BETWEEN_WORDS (3*CLOCKS_PER_SEC/10)

extern unsigned char _INTROCHARSET_LOAD__[];
extern unsigned char _LOGOBITMAP1_LOAD__[];
//defined in linker config file

extern bool quit;//defined in main program

void writetext(unsigned char* bitmappos){
	//"0123456789012345678901234567890123456789"	
	//maximum width = 40 chars
	/*Dont use Tabs!!!!!!!*/
	//One text line must not be longer than this:
	//      "01234567890123456789012345678901234567\n"
	static unsigned char text[] = {
					"    the lurking leader of cascade\n"
					"  also known as the mighty and noble\n"
					"               zo$%tleff\n"
					"      finally raised his paw...\n"
					"          ...and we jumped\n"
					"            to serve him!\n"
					"*"
					"    after a long, long time we are\n"
					"   able to deliver some hand-picked\n"
					"     bits'n'bytes, that hopefully\n"
					" entertain you for a minute or two...\n"
					" oh 'n btw... we baptized this brood:\n"
					"          \"se gitai o ilios\"\n"
					"*"
					"  this is not a big production as we\n"
					" (and maybe you) preferably watch but\n"
					"we wished to release something just to\n"
					"give at least a little sign of life...\n"
					"   yes, we became rusted, old farts\n"
					"        but we're still around!\n"
					"*"
					"  amazing how long it took to finish\n"
					" this small demo. how the hell did we\n"
					"     do all this stuff back then,\n"
					"   when we were young and innocent?\n"
					"   still it was really much fun to\n"
					"     poke and pixel once again...\n"
					"*"
					" now over to the unavoidable credits:\n"
					"coding performed: monte carlos/cascade\n"
					"  grafical handcraft: zeldin/cascade\n"
					"next part music: icebreaker/ex-cascade\n"
					"and to pay tribute, we took this music\n"
					" done by: matrix/nordic beat (r.i.p.)\n"
					"*"
					"some cold bottles of hessian applewine\n"
					"to: mr.lee, axis, ray, saigon, lakhvir\n"
					"zottleff, jojo, zyron, sabitha, patchi\n" 
					"sge + attila, diver, tch, kajol, scoup\n"
					"violator, indy 1933, zore, dynamic duo\n"
					"shahrukh khan, rap and dr. tancredi...\n" 
					"*"
					"fuckings to timbalamer, gez, ifpi + co\n"
					"                  ---\n"
					"you reached the last snippets of text.\n"
					"have fun wherever you are right now 'n\n"
					"don't forget to join the piratpartiet!\n"
					"        may the sun be with you!\n"
					"*#"};

	/*text for the intro is diveded into sections which
		are seperated by a star and ended with #. 
		See text-file. Only one section
		is shown between successive logo-fadeins/fadeouts.
	*/
	
	static unsigned char edgeoffsets[] = OFFSETS1x2s;
	NXN_CHAR_TYPE introcharset = 
				{_INTROCHARSET_LOAD__, 1, 2, 27, edgeoffsets};
				//charadr, xsize, ysize, first, offsets
	/*define, where to find the charset and how the chars are
		made up*/
					
	static unsigned int textpos=0;
	unsigned char curchar;
	bool succspace = false;
	/*variables for identifying the next char to be printed*/
	
	unsigned char xpos=0;
		
	for(; (curchar=text[textpos])!='*'; textpos++){ 
		switch(curchar){	
			case '\n':{
				succspace = false;
				bitmappos+=((40-xpos)<<3)+0x140;
				xpos=0;
				break;
			}//jump to next line, if linebreak
			case ' ':{
				if(!succspace){
					succspace = true;
					delay_loop(TIME_BETWEEN_WORDS);
				}//overread sucessive spaces
				bitmappos += 8;
				++xpos;
				break;
			}
			default: {
				succspace=false; 
				putnxnchar(&introcharset, curchar , &bitmappos);
				++xpos;
			}
		}
		
		if( quit ){
			return;
		}//if quit pressed during writing the text,
		 //then return to main and start fade-off
	}
	++textpos;

	delay_loop(2*CLOCKS_PER_SEC);
		
	if( text[textpos] == '#' ){
		quit = true;
	}//End char reached so set quit flag
}