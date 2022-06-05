#include <conio.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define ARGAMOUNT 4

/*This program implements the midpoint algorithm for circles used in the book 
	"Grundlagen der Computergrafik" by Addison Wesley.
	As a base you use the Phythagoras relation f(x,y)=xx+yy-rr.
	If f gets negative you are inside the circle, zero you are on and 
	positive you are outside. Considering the value f(x+1,y+1/2) is pos/neg
	you choose either the right pixel or the right/lower pixel for the first 
	oktant of the circle. If you substract f(x_new,y_new) from f(x,y) you get
	the initial deltaE and deltaSE. If you build the difference of second order,
	which is deltaE_new-deltaE, in all four combination if E and SE you get
	the increments 2,2,2,4.*/

void SetPoint(unsigned short x,unsigned short y, unsigned short midpointx, 
			unsigned short midpointy, char symbol){
	gotoxy(x+midpointx,y+midpointy);cputc(symbol);
	gotoxy(midpointx-x,y+midpointy);cputc(symbol);
	gotoxy(x+midpointx,midpointy-y);cputc(symbol);
	gotoxy(midpointx-x,midpointy-y);cputc(symbol);
	gotoxy(y+midpointx,x+midpointy);cputc(symbol);
	gotoxy(midpointx-y,x+midpointy);cputc(symbol);
	gotoxy(y+midpointx,midpointy-x);cputc(symbol);
	gotoxy(midpointx-y,midpointy-x);cputc(symbol);
}

void MidPointCircle(unsigned short Radius, unsigned short midpointx, 
			unsigned short midpointy, char symbol){

	unsigned short x,y,deltaE,deltaSE;
	short d;

	x=0;
	y=Radius;
	d=1-Radius;

	deltaE=3;
	deltaSE=5-Radius*2;
	SetPoint(x,y,midpointx,midpointy,symbol);
	while(y>x){
		if(d<0){
			d+=deltaE;
			deltaE+=2;
			deltaSE+=2;
			x++;
		}else{
			d+=deltaSE;
			deltaE+=2;
			deltaSE+=4;
			x++;
			y--;
		}
		SetPoint(x,y,midpointx,midpointy,symbol);
	}
}

void main(int argc, char* argv[]){
	unsigned short r,x,y;
	unsigned char maxx,maxy;

	if(argc!=ARGAMOUNT){printf("Wrong number of args");exit(EXIT_FAILURE);}
	r=atoi(argv[1]);
	x=atoi(argv[2]);
	y=atoi(argv[3]);
	screensize(&maxx,&maxy);

	if(((int)(x-r)<0)|((int)(y-r)<0)){
		printf("Radius too large");exit(EXIT_FAILURE);
	}
	if(((int)(x+r)>maxx)|((int)(y+r)>maxy)){
		printf("X or Y too large");exit(EXIT_FAILURE);
	}
	if(r==0){
		for(;r<12;r++){
			MidPointCircle(r,x,y,'*');
		}
	}
	MidPointCircle(r,x,y,'*');
}
