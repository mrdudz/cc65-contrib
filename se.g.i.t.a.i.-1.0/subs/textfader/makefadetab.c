void makefadetab(unsigned char xmax, unsigned char ymax,
								 unsigned char offset, register unsigned char* dest){
	unsigned char xpos;
	unsigned char ypos;
	unsigned char rasterstate;
	
	for (ypos = 0; ypos < ymax; ypos++){
		for (xpos = 0; xpos < xmax; xpos++){
			rasterstate = (xpos^ypos)&1;
			*dest= (rasterstate==1)?((xpos>>1)+offset):xpos>>1;
			++dest;
		}
	}
}
