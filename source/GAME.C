#include <stdio.h>
#include <stdlib.h>

#define	KEYBOARDPORT	9
#define	TIMERPORT		8
#define RIGHT			77
#define LEFT			75
#define SPACE			57
#define	XPOS			20
#define YPOS			20
#define	YPLUS			5

typedef struct blox *bloxptr;       //Block structure
typedef struct blox{
	int xpos;
	int ypos;
	int speed;
	bloxptr next;
}blox;

typedef struct bullet *bulletptr;   //Bullet structure
typedef struct bullet{
	int xpos;
	int ypos;
	int speed;
	bulletptr next;
}bullet;

typedef struct hero *heroptr;       //Hero structure
typedef struct hero{
	int xpos;
	int ypos;
	int score;
}hero;

//Lists, hero and realm
heroptr heman;              //Hero
bulletptr bullethead;       //Header for bulletlist
bloxptr blockhead;          //Header for blocklist
char realm[XPOS][YPOS];     //Realm
int close = 0;              //If ESC key pressed, close variable changes to 1
int timestroke = 0;         //Each time stroke this variable increases

//Initialize Hero
void createHero(){
	heman = (heroptr) malloc(sizeof(hero));
	heman->xpos = 9;
	heman->ypos = YPOS+YPLUS-1;
	heman->score = 0;
}

//Initialize header for bullet list
void createBulletHead(){
	bullethead = (bulletptr) malloc(sizeof(bullet));
	bullethead->speed = -1;
	bullethead->xpos = -1;
	bullethead->ypos = -1;
	bullethead->next = NULL;
}

//Initialize header for block list
void createBlockHead(){
	blockhead = (bloxptr) malloc(sizeof(blox));
	blockhead->speed = 1;
	blockhead->xpos = -1;
	blockhead->ypos = -1;
	blockhead->next = NULL;
}

//Adds bullet to bullet list
void addBullet(int xpos){
	bulletptr tmp = bullethead;
	bulletptr bul = (bulletptr) malloc(sizeof(bullet));
	bul->speed = 3;
	bul->xpos = xpos;
	bul->ypos = YPOS+YPLUS-2;
	bul->next = NULL;
	for (; tmp->next; tmp = tmp->next );
	tmp->next = bul;
}

//Adds block to block list
void addBlox(int xpos,int speed){
    bloxptr tmp = blockhead;
	bloxptr blx = (bloxptr) malloc(sizeof(blox));
	blx->speed = speed;
	blx->xpos = xpos;
	blx->ypos = YPLUS;
	blx->next = NULL;
	for(;tmp->next;tmp = tmp->next);
	tmp->next = blx;
}

//Updates the speed of blox
void updateSpeed(int newspeed){
    bloxptr tmp = blockhead;
	tmp->speed=newspeed;
	for ( ; tmp->next; tmp->next->speed=newspeed, tmp=tmp->next);
}

//Determines the pressed button
void action(int button){
	void changeVGA(char, int, int);
	bulletptr tmp= bullethead;
	int test = 0;
	if(button == RIGHT){										//If pressed button is right
		if(heman && (heman->xpos < 19)){
			heman->xpos++;	
		}
	}
	else if(button == LEFT){									//If pressed button is left
		if(heman && (heman->xpos > 0)){
			heman->xpos--;
		}
	}
	else if(button == SPACE){									//If pressed button is right
		if(heman){												//If hero exists
			if(realm[heman->xpos][heman->ypos-YPLUS-1] != '*'){	//If there is a bullet at the same coordinate don't add bullet
				for(;tmp->next;tmp=tmp->next){					
					if(tmp->next->ypos==heman->ypos-1){			//Don't add two bullet at the same horizantal coordinates
						test=1;
						break;
					}
				}
				if (test == 0)
					addBullet(heman->xpos);
			}
		}
	}
	else if(button == 1){
		close = 1;
	}
}

//Removes Block with given position from the list
void rmBlock(int xpos, int ypos){
	bloxptr rm, tmp=blockhead;
	for(;tmp->next;tmp=tmp->next){
		if (tmp->next->xpos == xpos && tmp->next->ypos == ypos) {
			rm = tmp->next;
			tmp->next = rm->next;
			free(rm);
			break;
		}
	}
}

//Removes Bullet with given position from the list
void rmBullet(int xpos, int ypos){
	bulletptr rm, tmp=bullethead;
	for(;tmp->next;tmp=tmp->next){
		if(tmp->next->xpos == xpos && tmp->next->ypos == ypos){
			rm = tmp->next;
			tmp->next = rm->next;
			free(rm);
			break;
		}
	}
}

//Removes All bullets from the list
void freeBullets(){
	bulletptr tmp=bullethead->next;
	for(;bullethead->next;){
		tmp=bullethead->next;
		bullethead->next = bullethead->next->next;
		free(tmp);
	}
}

//Removes All blox from the list
void freeBlox(){
	bloxptr tmp=blockhead->next;
	for(;blockhead->next;){
		tmp=blockhead->next;
		blockhead->next = blockhead->next->next;
		free(tmp);
	}
}

//Prints given char to given coordinates to screen
void changeVGA(char c, int xpos, int ypos){
	int current = (( (ypos) * 80)+xpos)*2;
	_asm{
		mov ax, 0B800h
		mov es, ax
		mov bx, current
		mov al, c
		mov ah, 07h
		mov word ptr es:[bx], ax
	}
}

//Creates the realm
void createRealm(){
	int x=0, y=0;
	for (y = YPLUS; y < 25; y++) {
		for (x = 0; x < XPOS ; x++) {
			changeVGA(' ', x, y); 
		}
	}
}

//Empty the realm
void fillDOS(){
	int x=0, y=0;
	for (y = 0; y < 25 ; y++) {
		for (x = 0; x < 80; x++) {
			changeVGA(178, x, y);
		}
	}
}

//Updates the block list
void updateBlox(){
	bloxptr tmp = blockhead;
	for (;tmp->next;tmp=tmp->next) {					//Move blocks to next coordinate
		if(tmp->next->ypos < 19+YPLUS)					//If it's coordinate is not same with hero move
			tmp->next->ypos++;
		else											//Else close the game
			close = 1;
	}
}

//Updates the bullet list
void updateBullets(){
	bulletptr tmp = bullethead;
	for (; tmp->next;tmp=tmp->next) {					//Move bullets to next coordinate
		tmp->next->ypos--;
	}
	tmp = bullethead;
	for (;tmp->next; tmp=tmp->next)
		if(tmp->next->ypos-YPLUS < 0)					//If bullet is out of realm delete it
			rmBullet(tmp->next->xpos,tmp->next->ypos);
}

//Fills the array with updated lists.
void fillArray(){
	bloxptr bltmp = blockhead;
	bulletptr butmp = bullethead;
	int i=0, j=0;
	for (i = 0; i < XPOS; i++)												//Fill realm with empty char
		for (j = 0; j < YPOS; j++)
			realm[i][j]=' ';
	for(;bltmp->next; bltmp=bltmp->next){									//Fill realm with current blox
		if(bltmp->next->ypos != YPOS+YPLUS-1)								//If it's coordinate is not same with hero fill array
			realm[bltmp->next->xpos][bltmp->next->ypos-YPLUS]='#';
		else																//Else close the game
			close = 1;
	}
	for (;butmp->next;butmp=butmp->next) {									//Fill realm with current bullets
		if(realm[butmp->next->xpos][butmp->next->ypos-YPLUS+1]=='#'){		//If there is a block behind bullet remove both bullet and block
			rmBlock(butmp->next->xpos,butmp->next->ypos+1);
			rmBullet(butmp->next->xpos, butmp->next->ypos);
			heman->score++;
			if(heman->score%10 ==0)											//If modulus is 0 update the blox' speed
				updateSpeed(blockhead->speed+1);
		}
		if (realm[butmp->next->xpos][butmp->next->ypos-YPLUS]== '#'){		//If there is block with the same coordinate remove both bullet and block
			rmBlock(butmp->next->xpos,butmp->next->ypos);
			rmBullet(butmp->next->xpos,butmp->next->ypos);
			heman->score++;
			if(heman->score%10 == 0)										//If modulus is 0 update the blox' speed
				updateSpeed(blockhead->speed+1);
		}
		if(butmp->next->ypos-YPLUS >= 0)									//If bullet is in the realm fill array with it.
			realm[butmp->next->xpos][butmp->next->ypos-YPLUS] = '*';
	}
	realm[heman->xpos][heman->ypos-YPLUS]='H';								//Add hero to realm
}

//Draws realm to screen
void drawRealm(){
	int xpos,ypos;
	for (ypos = 0; ypos < YPOS ; ypos++) {
		for (xpos = 0; xpos < XPOS ; xpos++) {
			changeVGA(realm[xpos][ypos],xpos, ypos+YPLUS);
		}
	}
}

//Prints score to dos screen
void hitcounter(){
	int num=heman->score, i=0;
	changeVGA(' ',22,5);
	changeVGA(' ',21,5);
	changeVGA(' ',20,5);
	if(num==0){
		changeVGA('0',22,5);
		return;
	}
	for( ;num>0;i++,num/=10 ){
		int digit = num%10;
		changeVGA((digit+'0'), 22-i, 5);
	}
}

//Helps to generate random blox
int myrand(){
	float rd = (float)timestroke;
	rd = rd * 1103515245 +12345;
    return (unsigned int)((rd + 5221) / 65536 ) % 32768; 
}

//Helps to find target
void target(){
	int xpos;
	for(xpos=0;xpos<20;xpos++)
		changeVGA(178,xpos, YPLUS-1);
	changeVGA('|',heman->xpos,YPLUS-1);
}

//Returns the given interrupt procedure's vector
void interrupt ( *oldVector(int port) )(){
	void interrupt (*procedure)();
	_asm{
		cli
		xor ax, ax
		mov es, ax
		mov ax, port
		mov dx, 4
		mul dx
		mov bx, ax
		mov dx,es:[bx]
		mov word ptr procedure, dx
		mov dx, es:[bx+2]
		mov word ptr procedure+2, dx
		sti
	}
	return procedure;
}

//Replaces with my interrupt vector
void newVector(int port, void interrupt (*procedure)() ){
	_asm{
		cli
		xor ax, ax
		mov es, ax
		mov ax, port
		mov dx, 4
		mul dx
		mov bx, ax
		mov ax, word ptr procedure
		mov word ptr es:[bx], ax
		mov ax, word ptr procedure+2
		mov word ptr es:[bx+2], ax
		sti
	}
}

//Handles timer interrupts
void interrupt timer(){
	_asm{
		cli
		mov al, 0x02
		out 0x43, al
		mov ax, 0xFFFF
		out 0x40, ax
		sti
	}
	timestroke++;																			//Each timer interrupt it increases
	if(close==0){
		if( timestroke%6 == 0){																//Move bullets three times a sec
			updateBullets();
		}	
		if ( ((18/blockhead->speed) != 0) && (timestroke%(18/blockhead->speed)==0)) {		//Move blox according to their speeds
			updateBlox();
		}
		if (timestroke%(18*5) == 0 || timestroke==1) {										//Add random block each five sec
			addBlox((myrand()%20),bullethead->speed);
		}
		fillArray();																		//Fill array with updated bullets, blox, hero
		drawRealm();																		//Draw that array to realm
		target();																			//Help to shoot block
		hitcounter();																		//Print score
	}
	_asm{
		cli
		push ax
		mov al, 20h
		out 20h, al
		pop ax
		sti
	}
}

//Handles keyboard interrupts
void interrupt keyboard(){
	int button;
	_asm{
		cli
		in al, 0x60
		xor ah, ah
		mov button, ax
		in al, 0x61
		or al, 0x82
		out 0x61, al
		and al, 0x7f
		out 0x61, al
		mov al, 0x20
		out 0x20, al
		sti
	}
	action(button);				//Gets button's integer value
}

//Let the game begin
int main(){
	void interrupt (*timeIntVect)();
	void interrupt (*keyboardIntVect)();
	keyboardIntVect = oldVector(KEYBOARDPORT);      //Backup old keyboard interrupt
	timeIntVect = oldVector(TIMERPORT);             //Backup old timer interrupt						
	newVector(KEYBOARDPORT,keyboard);               //Set new keyboard interrupt
	newVector(TIMERPORT,timer);                     //Set new timer interrupt
	fillDOS();                                      //Fill dos screen with specific character          
	createRealm();                                  //Create realm at bottom left
	createHero();                                   //Create a hero
	createBlockHead();                              //Create the header for blocklist
	createBulletHead();                             //Create the header for bulletlist
	while(close == 0){}                             //Infinite loop until close variable changes
	newVector(KEYBOARDPORT,keyboardIntVect);        //Restore old keyboard interrupt
	newVector(TIMERPORT,timeIntVect);               //Restore old timer interrupt
	return 0;
}

