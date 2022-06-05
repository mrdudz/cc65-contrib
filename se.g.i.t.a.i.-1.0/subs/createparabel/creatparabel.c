void creatparabel(unsigned char* dest, unsigned int maxsqrt, 
									unsigned char lendivide){
	unsigned int cursqrt=0;
	unsigned char cnt=0;
	
	do{
		dest[cnt] = ((long int)cursqrt*cursqrt)>>24 ;
		cursqrt += lendivide*maxsqrt;
		++cnt;
	}while(cnt != (unsigned char)(256/lendivide));
}

