#include "iGraphics.h"
#include "DrawManager.hpp"
#include "CollisionManager.hpp"
#include "TemporalCoupling.hpp"
#include "MainMenu.hpp"


// Stored pointers for direct reference to specific objects
GameObject* player1 = 0;
GameObject* player2 = 0;


// Global image variables
int player1Img = -1;
int player2Img = -1;
int switch1Img = -1;
int effect1Img = -1;
int switch2Img = -1;
int effect2Img = -1;

// Images
int imgT, imgL1, imgL2, imgR1, imgR2;


// Structures
	//quest
	typedef struct{
		int img;
	}Quest;
	//2.1 - object constructing
	Quest player1Quest[5], player2Quest[5];

	
int gameState = 0; //Main Menu -> 0, Play -> 1, Story -> 2, Leaderboard -> 3, Options -> 4, Credits -> 5
int screenWidth = 1920, screenHeight = 980;
double speed = 5.0;
int offset = 5;


//individual screen
int screenW = 920, screenH = 650;
int questBarH = 90;
//screen player1
int screen1X = 20, screen1Y = 20;
//screen p2
int screen2X = 980, screen2Y = 20;


//map indicator
int left = 1;
int right = 1;


void mapLoader1(){
	//left screen
	if(left == 1){
		iShowImage(screen1X+offset, screen1Y+offset, screenW-2*offset, screenH-2*offset, imgL1);
	}else if(left == 2){
		iShowImage(screen1X+offset, screen1Y+offset, screenW-2*offset, screenH-2*offset, imgL2);
	}
}

void mapLoader2(){
	//right screen
	if(right == 1){
		iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, imgR1);
	}else if(right == 2){
		iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, imgR2);
	}
}

void questLoader(){
	iShowImage(screen1X+offset, screen1Y+screenH, screenW-2*offset, questBarH-offset, player1Quest[0].img);
	iShowImage(screen2X+offset, screen2Y+screenH, screenW-2*offset, questBarH-offset, player2Quest[0].img);
}


//built-in function structure
void iDraw(){
	
	iClear();

	if (DoitoProhorMenu::isMenuState(gameState))
	{
		DoitoProhorMenu::draw(gameState, screenWidth, screenHeight);
		return;
	}

	//Title
	iShowImage(460, 770, 1000, 200, imgT);

	if(gameState == 0){

	}else if(gameState == 1){
		//player 1 map
		iSetColor(168, 169, 173);
		iFilledRectangle(screen1X, screen1Y, screenW, screenH);
		mapLoader1();
	
		//player 2 map
		iSetColor(168, 169, 173);
		iFilledRectangle(screen2X, screen2Y, screenW, screenH);
		mapLoader2();
	
		//quest bar
		iSetColor(168, 169, 173);
		iFilledRectangle(screen1X, screen1Y+screenH, screenW, questBarH);
		iSetColor(168, 169, 173);
		iFilledRectangle(screen2X, screen2Y+screenH, screenW, questBarH);
		questLoader();


		// Draw all switches and effects (always in the background)
		GameObject* bgList[MAX_OBJECTS];
		int bgCount = 0;
		getBackgroundDrawList(bgList, &bgCount);

		for (int i = 0; i < bgCount; i++) {
			// Skip rendering effects if they are not activated
			if (bgList[i]->id == OBJ_EFFECT && !bgList[i]->isActivated) continue;
			
			if (bgList[i]->imgIndex != -1) {
				iShowImage(bgList[i]->x, bgList[i]->y, bgList[i]->width, bgList[i]->height, bgList[i]->imgIndex);
			}
		}

		// Get the sorted sequence of objects (excluding switches and effects) from DrawManager
		GameObject* drawList[MAX_OBJECTS];
		int currentCount = 0;
		getSortedDrawList(drawList, &currentCount);

		// Draw everything
		for (int i = 0; i < currentCount; i++) {
			if (drawList[i]->imgIndex != -1) {
				iShowImage(drawList[i]->x, drawList[i]->y, drawList[i]->width, drawList[i]->height, drawList[i]->imgIndex);
			}
		}
		


	}else if(gameState == 2){

	}else if(gameState == 3){

	}else if(gameState == 4){

	}else if(gameState == 5){

	}

}

void iMouseMove(int mx, int my) {}

void iPassiveMouseMove(int mx, int my) {}

void iMouse(int button, int state, int mx, int my) {
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){

		if (DoitoProhorMenu::isMenuState(gameState))
		{
			DoitoProhorMenu::handleClick(gameState, mx, my);
			return;
		}
		
	}
	
	
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		
	}
}


// Special Keys:
// GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6, GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12, 
// GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP, GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT

void fixedUpdate() {
	// Keys allocated for player 1
	if(player1 != 0){
		double nextX = player1->x;
		double nextY = player1->y;

		if ( isKeyPressed('d')){
			player1->imgIndex = iLoadImage("Image//P1R.png");
			nextX += speed;
		} // Player 1 right
		if (isKeyPressed('w')){
			player1->imgIndex = iLoadImage("Image//P1B.png");
			nextY += speed;
		} // Player 1 up
		if (isKeyPressed('a')){
			player1->imgIndex = iLoadImage("Image//P1L.png");
			nextX -= speed;
		} // Player 1 left
		if (isKeyPressed('s')){
			player1->imgIndex = iLoadImage("Image//P1F.png");
			nextY -= speed;
		} // Player 1 down
		if (!checkCollision(player1, nextX, nextY)) {
            player1->x = nextX;
            player1->y = nextY;
        } // If no collision then update the x & y values
		if (isKeyPressed('e')){
			triggerRemoteEffect(player1, 25.0);
		} // Trigger the OBJ_EFFECT for the OBJ_SWITCH on the coordinate of player 1

	}

	// Keys allocated for player 2
	if(player2 != 0){
		double nextX = player2->x;
		double nextY = player2->y;

		if (isSpecialKeyPressed(GLUT_KEY_RIGHT)){
			player2->imgIndex = iLoadImage("Image//P2F.png");
			nextX += speed;
		} // Player 2 right
		if (isSpecialKeyPressed(GLUT_KEY_UP)){
			player2->imgIndex = iLoadImage("Image//P2F.png");
			nextY += speed;
		} // Player 2 up
		if (isSpecialKeyPressed(GLUT_KEY_LEFT)){
			player2->imgIndex = iLoadImage("Image//P2F.png");
			nextX -= speed;
		} // Player 2 left
		if (isSpecialKeyPressed(GLUT_KEY_DOWN)){
			player2->imgIndex = iLoadImage("Image//P2F.png");
			nextY -= speed;
		} // Player 2 down
		if (!checkCollision(player2, nextX, nextY)) {
            player2->x = nextX;
            player2->y = nextY;
        }// If no collision then update the x & y values
		if (isKeyPressed('0')){
			triggerRemoteEffect(player2, 25.0);
		} // Trigger the OBJ_EFFECT for the OBJ_SWITCH on the coordinate of player 2

	}
	


	if (isKeyPressed(' ')){
		// Playing the audio once
		//mciSendString("play ggsong from 0", NULL, 0, NULL);
	}
}


int main(){
	iInitialize(screenWidth, screenHeight, "Doito Prohor : The Time that Steals");
	
	DoitoProhorMenu::initialize();

	//1. Image
	//title
	imgT = iLoadImage("Image//Title.png");
	
	//quests
	//player 1 & 2 quests
	player1Quest[0].img = iLoadImage("Image//P1Q1.png");
	player2Quest[0].img = iLoadImage("Image//P2Q1.png");

	//initialize images of maps
	imgL1 = iLoadImage("Image//worldone1.png");
	imgL2 = iLoadImage("Image//Left2.jpeg");
	imgR1 = iLoadImage("Image//Right1.jpeg");
	imgR2 = iLoadImage("Image//Right2.jpeg");
	
	//initialize images of player 1 & 2
	player1Img = iLoadImage("Image//P1F.png");
	player2Img = iLoadImage("Image//P2F.png");

	//initialize images of switch and effects
	effect1Img = iLoadImage("Image//bridge1.png");
	switch1Img = iLoadImage("Image//platform1.png");
	
	//initialization
	//player 1 & 2
	player1 = createObject(OBJ_PLAYER, 460, 30, player1Img, 24, 48, 10, -1);
	player2 = createObject(OBJ_PLAYER, 1440, 30, player2Img, 24, 48, 10, -1);


	// Pair 1
	GameObject* effect1 = createObject(OBJ_EFFECT, 400, 500, effect1Img, 220, 60, 0, -1);
	createObject(OBJ_SWITCH, 150, 100, switch1Img, 30, 30, 0, getObjectIndex(effect1));

			
	iStart();
	return 0;
}