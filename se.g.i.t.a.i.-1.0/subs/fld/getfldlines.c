void logofader(void){
	static int i=255;
	static signed char direction = -1;
	
	unsigned char numfldlines;

	numfldlines = parabeltab[i];

	i+=direction;

	if ((i<0)|(i>>8)){
		direction = -direction;
		i+=direction;	
	}
}