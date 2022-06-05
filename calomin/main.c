/*Caverns of the lost miner version D*/
/*2007 Baktra, under GNU LGPL*/

/*Memory usage :
  
  Raster Music tracker:                         : -8192-? A bit of memory
  Program: (11k)                                : 11776-22900
  Cave display list (190 bytes,k boudnary)      : 24576-24766 PAGE:96 OFFSET:0
  RMT file (418 bytes):                         : 25600-? A bit of memory PAGE 100

  PMG one-line resolution (2k)                  : 30720-32767 PAGE:120
  -
  Cave display memory (22x40=880 bytes)         : 33104-33983 PG: 129,OFS: 80
  Second character set (1024 bytes)             : 34816-35839 PAGE:136

  Menu display memory(960 bytes)                : 40000-40959 
  Cave status bar (40 bytes)                    : 40144-40183
  -
  Cave layout - 14 caves(6160 bytes)            : 41000-46719
  -
  Memory locations rearanged. No problem with DOS 2.5, FLOP DOS, TOS.4.1 BL+, DOS XE.
  Sorry for previous versions. They were unusable probably under any DOS. Simply
  I didn't follow BEWESOFT's advice: "Do not use Turbo and don't think in Turbo"
  Next time, i'll do better

  Thanks to: Raster/c.p.u (for advice and publishing version C in FLOP)
             MatoSimi     (for his Font editor)
 
  Changes:
  - ANTIC mode 4 is used to display caves. It's faster and consumes
    much less memory. We have also 5 colours available.

  - Preprocessor defines are better used
  
*/

/*Memory layout*/
#define CAVDMEM 33104U
#define CAVLAYOUT 41000U
#define PMGPAGE 120U
#define FONTPAGE 136U
#define PMGSTART 30720U
#define PMGEND 32767U
#define SBMEM 40144U
#define CAVDL_LO 0
#define CAVDL_HI 96

/*Version dependent*/
#define CALOMIN_VERSION "D"
#define MAX_CAVE_INDEX 12
#define NUMBER_OF_CAVES 13

#define CTRL_DELAY 320
#define CTRL_RESTDELAY 360

/*Joystick*/
#define JS_L 11
#define JS_R 7
#define JS_U 14
#define JS_D 13
#define JS_C 15

#include <stdio.h>
#include <conio.h>
#include <peekpoke.h>
#include <stdlib.h>
#include <string.h>

#define MENU_RESULT_STAY 1
#define MENU_RESULT_QUIT 2
#define MENU_RESULT_BEGIN 3

void doGame();
void paintElement(unsigned char x,unsigned char y,unsigned char elem);
void paintCave();
void caveToArray(unsigned char cv);
void pmgInit();
void pmgSetPos(unsigned char x,unsigned char y);
void delay(unsigned int w);
void doGratulation();
void doGameOver();
void setTextModeSettings();

void rmtInitMenuMusic();
void rmtInitGameMusic();
void rmtPlayDiamond();
void rmtPlayPicked();
void rmtPlayDeath();
void rmtPlayUpdate();
void rmtPlayGratulation();
void rmtPlayJump();
void rmtSetVBI();
void rmtAllStop();
void rmtSuspend();
void rmtResume();
void rmtRestoreVBI();

void asmReboot();

unsigned char moveLeft();
unsigned char moveRight();
void moveUp();
void moveDown();
unsigned char jumpUp();	/*Return 0 if OK, 1 if death*/
void fallDown();
void handleHighJump();
void updateStatusBar();
unsigned char checkTreasure();
void checkDeath();

unsigned char maxCaveReached;	/*Max. warp*/
unsigned char startingCave;	/*Warp*/
unsigned char dmactlStore;	/*559 Store*/
unsigned char menuResult;	/*User asks to exit or start game*/
unsigned char lives;	/*Lives*/

unsigned char currentCave;	/*Current cave*/
unsigned char treasure;		/*How many diamonds in cave*/
unsigned char collected;	/*How many diamonds collected*/
unsigned char caveDeath;	/*1 for death*/
unsigned char caveAllPicked; /*1 for all picked*/
unsigned char stayHere;	/*Stay in cave*/
unsigned char overType; /*0=user's quit, 1 game over, 2 congratulations*/

/*Menu displaylist adress*/
unsigned char menuDl1;
unsigned char menuDl2;

/*Temporary*/
unsigned char x1;
unsigned char y1;
unsigned int i1;
unsigned int i2;
unsigned char z1;

/*Actual cave status*/
unsigned char cave[20][22];
unsigned char caveBroken [20][22];

/*Miner location*/
unsigned char minx,miny;
/*Miner - PMG P0*/
unsigned char minerData[] = {60,126,90,219,255,195,102,60};

/*Difficulty strings*/
char* difficultyNames[] = {"  NORMAL","MODERATE","    EASY"};
/*Lives ammounts*/
unsigned char difficultyLives[] = {4,5,5};

/*For high jump*/
unsigned char hijs;
unsigned char hiJump;

/*Movement control - allocated in asm source*/
extern unsigned char mvDelay;
extern unsigned char dliHandler;

/*Movement speed setup - this is affected by difficulty*/
unsigned char controlSpeed;	/*This hold the difficulty 0 Normal,1 MODERATE,2 Easy*/
unsigned char brokenSpeed;
unsigned char hijumpSpeedA;
unsigned char hijumpSpeedB;
unsigned char controlDelay;
unsigned char fallSpeed;


/*Passable objects*/
/*                          0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2  */
/*                          0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9  */
unsigned char passable[]=  {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1};
unsigned char notJump []=  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1};
unsigned char broken[]  =  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char deathable[]= {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0};
/*                          R D R R R R S L B             B I S S S D D D             B 
                            O E O O O O T A R             R M T K K I I I             L 
                            C A C C C C A D O             O M R U U A A A             A 
                            K T K K K K L D K             K F T L L M M M             N
                                            1 2 3 4 5 6 7 8
*/

/*Element-starting character mapping*/
unsigned char map[] = 
{
	64+128, /*ROCK*/
	66+128, /*DEATH*/
	68+128, /*ROCK*/
	70+128, /*ROCK*/
	72+128, /*ROCK*/
	74+128, /*ROCK*/
	76+128, /*STALACT.*/
	78, /*LADDER*/
	97+128, /*BROKEN 1*/
	99+128, /*BROKEN 2*/
	101+128, /*BROKEN 3*/
	103+128, /*BROKEN 4*/
	105+128, /*BROKEN 5*/
	107+128, /*BROKEN 6*/
	109+128, /*BROKEN 7*/
	111+128, /*BROKEN 8*/
	90+128,	 /*IMMEDIATE FALL*/
	3,	/*STARTING POSITION*/
	80,	/*SKULL 1*/
	82, /*SKULL 2*/
	84,	/*DIAM 1*/
	86, /*DIAM 2*/
	88, /*DIAM 3*/
	3,3,3,3,3,3, /*SPARE*/
	0,	/*BLANK*/
};

/*PMG P0's coordinates*/
unsigned char p0x,p0y;

/*This draws the main menu*/
void drawMainMenu() {
	unsigned char px;
	/*Disable view*/
	dmactlStore=PEEK(559);
	
	POKE(559,0);
	POKE(560,menuDl1);
	POKE(561,menuDl2);
	
	setTextModeSettings();

	clrscr();
	/*Plain texts*/
	cputsxy(7,1,"CAVERNS OF THE LOST MINER");
	cputsxy(11,2,"BAKTRA (LGPL) 2007");
	
	cputsxy(2,19,"Guide the miner through 14 dangerous");
	cputsxy(2,20,"caves. Stay alive and collect all");
	cputsxy(2,21,"diamonds in each cave.");
	cputsxy(38,21,CALOMIN_VERSION);
	cputsxy(2,9,"Press");
	cputsxy(14,9,"to start game");
	cputsxy(2,12,"Press");
	cputsxy(10,12,"to set starting cave:");
	cputsxy(2,14,"Press");
	cputsxy(10,14,"to set difficulty:");
	/*Inverse texts*/
	revers(1);
	cputsxy(8,9,"START");
	cputsxy(8,12,"C");
	cputsxy(8,14,"D");
	revers(0);
	/*Lines*/
	for(px=1;px<39;px++) {
		cputcxy(px,0,CH_HLINE);
		cputcxy(px,3,CH_HLINE);
		cputcxy(px,22,CH_HLINE);
		cputcxy(px,18,CH_HLINE);
	}
	/*Starting cave*/
	gotoxy(35,12);
	cprintf("%2d",startingCave);
	/*Control mode*/
	cputsxy(29,14,difficultyNames[controlSpeed]);
	
	cursor(0);
	POKE(559,dmactlStore);
}



int main() {
	
	overType=0;
	clrscr();
	maxCaveReached=0;
	controlSpeed=0;
	
	/*Cheat/debug helper*/
	if (PEEK(53279U)==5) {
		maxCaveReached=MAX_CAVE_INDEX;
	}
	
	/*Initialize our VBI routine*/
	rmtSuspend();
	rmtSetVBI();
	
	menuDl1=PEEK(560);
	menuDl2=PEEK(561);
	
	cursor(0);
	
	/*Init the PMG*/
	pmgInit();
	
	/*Enclosing loop*/
	while(1) {
		
		/*Is this game over or Congratulations ?*/
		if (overType==1) doGameOver();
		if (overType==2) doGratulation();
		
		/*Menu music*/
		rmtSuspend();
		rmtInitMenuMusic();
		rmtResume();
		
		menuResult=MENU_RESULT_STAY;
		
		/*Draw main menu*/
		pmgSetPos(1,7);
		drawMainMenu();
				
		/*Swap keys*/
		if (kbhit()) cgetc();
		
		/*The menu loop*/
		while(menuResult==1) {
			
			/*Start game ?*/
			if (PEEK(53279U)==6) {menuResult=MENU_RESULT_BEGIN;POKE(77,0);}
			
			/*C -Increase starting cave number, ESC - quit, D - control mode*/
			if (kbhit()) {
				x1=cgetc();
				if (x1=='c') {
					if (startingCave<maxCaveReached) {
						startingCave++;
					}
					else {
						startingCave=0;
					}
					/*Redraw it*/
					gotoxy(35,12);
					cprintf("%2d",startingCave);
					continue;
				}
				if (x1==27) {
					rmtAllStop();
					rmtRestoreVBI();
					asmReboot();
					return 0;
				}
				if (x1=='d') {
					controlSpeed++;
					if (controlSpeed==3) controlSpeed=0;
					cputsxy(29,14,difficultyNames[controlSpeed]);
				}
			}
			
			
		}
		
		/*Here is the game*/
		/*Set appropriate control speed*/
		
		switch(controlSpeed) {
			/*Hard - classic*/
			case 0: {
				hijumpSpeedA=4;
				hijumpSpeedB=14;
				brokenSpeed=12;
				controlDelay=5;
				fallSpeed=3;
				break;
			}
			/*Easy - very slow*/
			case 2:	 {
				hijumpSpeedA=0;
				hijumpSpeedB=10;
				brokenSpeed=14;
				controlDelay=6;
				fallSpeed=4;
				break;
			}
			/*Medium - high jump is easier*/
			case 1: {
				hijumpSpeedA=2;
				hijumpSpeedB=13;
				brokenSpeed=12;
				controlDelay=5;
				fallSpeed=3;
				break;
			}
				
		}
		
		/*Set gameover*/
		overType=0;
		/*Setup the music*/
		rmtSuspend();
		rmtAllStop();
		rmtInitGameMusic();
		rmtResume();
		/*Game function*/
		doGame();
		
	}/*Enclosing loop*/
}

/******* THE GAME ENGINE **********/
void doGame() {
	
	unsigned char js;
	unsigned char probe;
	unsigned char fallTimer;
	unsigned char fallCounter;
	unsigned char probe2;
	unsigned char strig;
	unsigned char hijump;
	unsigned char fallLength;
	unsigned char fallFlag;
	unsigned char breakTimer;
	unsigned char caveQuit;
	unsigned char* dliadr;
	
	/*Setup the approriate display-list and charset*/
	POKE(560,CAVDL_LO);
	POKE(561,CAVDL_HI);
	POKE(756,FONTPAGE);
	
	/*Appropriate cave*/
	currentCave=startingCave;
	lives=difficultyLives[controlSpeed];
	
	/*DLI*/
	dliadr = &dliHandler;
	POKE(512,(unsigned int)dliadr%256);
	POKE(513,(unsigned int)dliadr/256);
	POKE(54286U,192);
	
	
	while(1) {
		
	/*Do not show anything*/
	dmactlStore=PEEK(559);
	POKE(559,0);
		
	/*Load cave into supplementary array*/
	caveToArray(currentCave);
	collected=0;
	/*Now we have also the miner's location in minx and miny and treasure*/
	
	/*Draw the cave*/
	POKE(709,12);
	POKE(710,134);
	POKE(708,242);   /*18*/
	POKE(711,244);	/*240*/
	
	paintCave();
	updateStatusBar();
	
	/*Place the miner*/
	pmgSetPos(minx,miny);
	
	/*Show the cave*/
	POKE(559,dmactlStore);
	
	/*Initialize counters*/
	stayHere=1;
	fallCounter=0;
	caveDeath=0;
	caveAllPicked=0;
	hijump=0;
	fallLength=0;
	fallFlag=0;
	caveQuit=0;
	
	while(stayHere || deathable[probe]==0) {
		
		/*Handle the quit commands*/
		if (PEEK(53279U)==6) { stayHere=0;caveQuit=1;POKE(77,0);break;}
		if (PEEK(53279U)==3) { caveDeath=1;stayHere=0;POKE(77,0);break;}
		
		/*Probe miner's surroundings*/
		probe2=cave[minx][miny];
		probe=cave[minx][miny+1];
		
		/*Under miner is blank space and we are not on the ladder and we don't perform high jump*/
		if (passable[probe]==1 && probe2!=7 && probe!=7) {
			if (fallTimer!=PEEK(20)) {
				fallCounter++;
				fallTimer=PEEK(20);
				if (fallCounter==fallSpeed) {
					fallDown();
					fallLength++;
					checkDeath();
					fallCounter=0;
					fallFlag=1;
					if (fallLength>6) {
						stayHere=0;
						caveDeath=1;
					}
				}
			}
		}
		else {
			fallCounter=0;
			fallLength=0;
			fallFlag=0;
		}
		
		/*Under miner is broken rock*/
		if (broken[probe]==1) {
			y1=miny+1;
			if (breakTimer!=PEEK(20)) {
				caveBroken[minx][y1]++;
				breakTimer=PEEK(20);
			}
			if (caveBroken[minx][y1]==brokenSpeed) {
				caveBroken[minx][y1]=0;
				if (probe<15) {
					cave[minx][y1]++;
					paintElement(minx,y1,cave[minx][y1]);
				}
				else {
					cave[minx][y1]=29;
					paintElement(minx,y1,cave[minx][y1]);
				}
			}
					
		}
		
		/*Under miner is unstable rock*/
		if (probe==16) {
			cave[minx][miny+1]=29;
			paintElement(minx,miny+1,29);
		}
		
		/*Miner surroundigs have been probed. Control*/
		if (mvDelay==0) {
			js=PEEK(632);
			strig=PEEK(644);
			switch(js) {
				/*Move right*/
				case 7: {
					
					if (strig==0 && !(notJump[probe])) {
						/*Medium jump to right*/
						rmtPlayJump();
						POKE(77,0);
						fallLength=0;
						if (jumpUp()) break;
						delay(CTRL_DELAY);
						if (jumpUp()) break;
						delay(CTRL_DELAY);
						moveRight();
						delay(CTRL_DELAY);
						moveRight();
						delay(CTRL_DELAY);
						moveRight();
						delay(CTRL_DELAY);
						checkDeath();
						break;
					}
					moveRight();
					POKE(77,0);
					checkDeath();
					break;
				}
				/*Move left*/
				case 11: {
					if (strig==0 && !(notJump[probe])) {
						/*Medium jump to left*/
						rmtPlayJump();
						POKE(77,0);
						fallLength=0;
						if (jumpUp()) break;
						delay(CTRL_DELAY);
						if (jumpUp()) break;
						delay(CTRL_DELAY);
						moveLeft();
						delay(CTRL_DELAY);
						moveLeft();
						delay(CTRL_DELAY);
						moveLeft();
						delay(CTRL_DELAY);
						checkDeath();
						break;
					}
					moveLeft();
					POKE(77,0);
					checkDeath();
					break;
				}
				
				
				/*Move down - only on the ladder*/
				case 13: {
					moveDown();
					POKE(77,0);
					checkDeath();
					break;
				}
				/*Move up - only on the ladder*/
				case 14: {
					if (strig==0) {
						if (notJump[probe]) break;
						fallLength=0;
						rmtPlayJump();
						POKE(77,0);
						handleHighJump();
						break;
					}
					moveUp();
					POKE(77,0);
					checkDeath();
					break;
				}
				
				default: {
					mvDelay=0;
				}
				
			}/*End switch js*/
		}
		
	}/*End of inner loop*/
	
	/*The cave has ended because of - treasure collected or death ----------------*/
	if (caveQuit) {
		overType=0;
		break;
	}
	if (caveAllPicked) {
		currentCave++;
		if (currentCave==NUMBER_OF_CAVES) {
			/*Hide the miner*/
			pmgSetPos(-8,32);
			/*Show congratulations screen before menu*/
			overType=2;
			break;
		}
		else {
			if (currentCave>maxCaveReached) maxCaveReached=currentCave;
		}
		/*Play positive sound*/
		rmtPlayPicked();
		continue;
	}
	if (caveDeath) {
		pmgSetPos(-8,32);
		paintElement(minx,miny,18);
		/*Play bad sound*/
		rmtPlayDeath();
		delay(1500);
		/*Check lives*/
		if (lives==0) {
			overType=1;
			break;
		}
		lives--;
		
		continue;
	}
		
	
	
	}/*End of outer loop*/
	
	
	/*Inhibit DLI*/
	POKE(54286U,96);
	
	
	
}

/******** SUPPLEMENTARY ************ ***********************
***********************************************************
**********************************************************/

/*Paint element at specific location*/
void paintElement(unsigned char x,unsigned char y,unsigned char elem) {
	
	i2=CAVDMEM+(y*40)+(x<<1);	/*Target memory*/
	/*Mapping for element*/
	z1=map[elem];
	
	POKE(i2,z1);
	POKE(i2+1,z1+1);
	
}

/*Paint whole cave*/
void paintCave() {
	
	for(y1=0;y1<22;y1++) {
		for(x1=0;x1<20;x1++) {
			paintElement(x1,y1,cave[x1][y1]);
		}	
	}
	
}

/*Move cave elements into array*/
void caveToArray(unsigned char cv) {
	/*Setup the array and treasure ammount*/
	unsigned char e;
	unsigned int store=440*cv+CAVLAYOUT;
	treasure=0;
	for(x1=0;x1<20;x1++) {
		for(y1=0;y1<22;y1++) {
			e=PEEK(store)-1;
			if (e==17) {
				e=29;
				minx=x1;miny=y1;	/*Miner's coordinates*/
			}
			if (e>19 & e<23) treasure++;
			cave[x1][y1]=e;
			store++;
		}
	}
	/*Clear the broken array*/
	memset(caveBroken,0,440);
}


/*Player missile graphics*/
void pmgInit() {
	unsigned int i;
	/*PMG memory start*/
	POKE(54279U,PMGPAGE);
	/*PMG single line*/
	POKE(559,62);
	/*Clear PMG memory*/
	for(i=PMGSTART;i<PMGEND;i++) {
		POKE(i,0);
	}
	/*Enable PMG*/
	POKE(53277U,3);
	
	
	/*Player 0 will be green*/
	POKE (704,166);
	
	/*Initial coordinates*/
	p0x=56;
	POKE (53248U,p0x);
	p0y=64;
	memcpy((unsigned char*)PMGSTART+1024+p0y,minerData,8);
}

void pmgSetPos(unsigned char x,unsigned char y) {
	unsigned char pom;
	p0x=48+x*8;
	pom=32+8*y;
	POKE(53248U,48+x*8);
	memset(((unsigned char*)p0y+PMGSTART+1024),0,8);
	memmove((unsigned char*)pom+PMGSTART+1024,minerData,8);
	p0y=pom;
}

/*Wait for some wsyncs*/
void delay(unsigned int w) {
	for(i1=0;i1<w;i1++) POKE(54282U,1);	
}


/*Move commands with range and pass checking*/
unsigned char moveLeft() {
	if (minx==0 || passable[cave[minx-1][miny]]==0) return 0;
	minx--;pmgSetPos(minx,miny);
	checkTreasure();
	mvDelay=controlDelay;
	return 1;
}

unsigned char moveRight() {
	if (minx==19 || passable[cave[minx+1][miny]]==0) return 0;
	minx++;pmgSetPos(minx,miny);
	checkTreasure();
	mvDelay=controlDelay;
	return 1;
}

void moveDown() {
	if (miny==21) return;
	x1=cave[minx][miny+1];
	if (passable[x1]==1) {
		miny++;pmgSetPos(minx,miny);
		checkTreasure();
		mvDelay=controlDelay;
	}
}

void fallDown() {
	if (miny==21) return;
	x1=cave[minx][miny+1];
	if (passable[x1]==1) {
		miny++;pmgSetPos(minx,miny);
		checkTreasure();
	}
}


void moveUp() {
	if (miny==0) return;
	x1=cave[minx][miny-1];
	/*Not free*/
	if (passable[x1]==0) return;
	
	/*We can move up only when we are on the ladder and up is passable element*/
	if (cave[minx][miny]==7) {
		/*Into death*/
		if (x1==6) {
			stayHere=0;
			caveDeath=1;
			return;
		}
	miny--;pmgSetPos(minx,miny);
	checkTreasure();
	mvDelay=controlDelay;
		
	}
	
}

unsigned char jumpUp() {
	if (miny==0) return 0;
	x1=cave[minx][miny-1];
	/*Into death*/
	if (x1==6) {
		stayHere=0;
		caveDeath=1;
		return 1;
	}
	/*Not free*/
	if (passable[x1]==0) return 0;
	
	miny--;pmgSetPos(minx,miny);
	checkTreasure();
	return 0;
}


/*This is special function to handle high jump.*/
void handleHighJump() {
	unsigned char hjc;
	unsigned char hjt;
	y1=0;	/*During high jump, only one steer move is allowed*/
	hjc=9;
	hiJump=3;
	mvDelay=0;
	while(hiJump>0) {
		if (jumpUp()) break;
		while(hjc<17) {
			if (mvDelay==0) {
				hijs=PEEK(632);
				switch(hijs) {
					case (JS_L): {
						if (y1) break;
						if(moveLeft()) y1=1;
						break;
					}
					case (JS_R): {
						if (y1) break;
						if(moveRight());y1=1;
						break;
					}
					default: {
						mvDelay=0;
						break;
					}
				}/*End of switch hiJS*/
				
			}
			if (hjt!=PEEK(20)) {hjt=PEEK(20);hjc++;}
		}
		if (hiJump==2) hjc=hijumpSpeedA; else hjc=hijumpSpeedB;
		
		hiJump--;
	}
	/*Jump is ended*/
	/*Check for death*/
	checkDeath();
	/*Check for unstable rock*/
	if (cave[minx][miny+1]==16) {
		cave[minx][miny+1]=29;
		paintElement(minx,miny+1,29);
	}
		
	
}

void checkDeath() {
	x1=cave[minx][miny+1];
	if (x1==1) {
		stayHere=0;
		caveDeath=1;
	}
}

unsigned char checkTreasure() {
	x1=cave[minx][miny];
	if (x1>19 && x1<23) {
		collected++;
		cave[minx][miny]=29;
		paintElement(minx,miny,29);
		rmtPlayDiamond();
		if (collected==treasure) {
			stayHere=0;
			caveAllPicked=1;
		}
		return 1;
	}
	return 0;
}

void updateStatusBar() {
	memset((unsigned char*)SBMEM,0,40);
	for(y1=2;y1<lives+2;y1++) {
		POKE(SBMEM+y1,224);
	}
	POKE(SBMEM,44);
	POKE(SBMEM+1,26);
	i1=SBMEM+40-NUMBER_OF_CAVES;
	for(y1=0;y1<currentCave+1;y1++) {
		POKE(i1,96);
		i1++;
	}
	
}

/*This is awful and unused*/
void sound(unsigned char a,unsigned char b,unsigned char c, unsigned char d) {
	POKE(53768U,0);
	POKE(53775U,3);
	POKE(53760U+(a<<1),b);
	POKE(53761U+(a<<1),(c<<4)+d);
	
}

/*Show Congratulations*/
void doGratulation() {
	
	/*Use the menu DL*/
	POKE(559,0);
	POKE(560,menuDl1);
	POKE(561,menuDl2);
	
	clrscr();    /*1234567890123456789012345678901234567890*/
	cputsxy(2,4,  "Congratulations !");
	cputsxy(2,6,  "The lost miner has passed all caves.");
	cputsxy(2,11, "Thank you for playing this game.");
	cputsxy(2,12, "Now you should create your own caves.");
	cputsxy(2,20, "Press SPACE to continue");
	
	setTextModeSettings();

	POKE(559,dmactlStore);
	delay(2500);
	rmtPlayGratulation();
	/*Swap keys*/
	if (kbhit()) cgetc();
	while(1) {
		if (cgetc()==' ') break;
	}
		
	
}

/*Show game over*/
void doGameOver() {
	/*Use the menu DL*/
	POKE(559,0);
	POKE(560,menuDl1);
	POKE(561,menuDl2);
	
	clrscr();    /*1234567890123456789012345678901234567890*/
	cputsxy(14,11, "GAME IS OVER");
	cputsxy(9,13, "Push SPACE to continue");
	setTextModeSettings();
		
	/*Show the screen again*/
	POKE(559,dmactlStore);
	delay(1000);
	/*Swap keys*/
	if (kbhit()) cgetc();
	while(1) {
		if (cgetc()==' ') break;
	}
	
}

/*Appropriate colours and charset*/
void setTextModeSettings() {
	POKE(710,176);
	POKE(709,14);
	POKE(756,224);
}
