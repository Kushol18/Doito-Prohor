#include "iGraphics.h"
#include "DrawManager.hpp"
#include "CollisionManager.hpp"
#include "TemporalCoupling.hpp"
#include "MainMenu.hpp"
#include "PauseMenu.hpp"
#include "PlayerAnimation.hpp"
#include "Collectables_Count.hpp"
#include "PlayerData.hpp"
#include "Leaderboard.hpp"
#include "CompanionManager.hpp"
#include "MapLoader.hpp"
#include "EntranceManager.hpp"
#include "CombatManager.hpp"
#include <cstring>

// Stored pointers for direct reference to specific objects
GameObject* player1 = 0;
GameObject* player2 = 0;
GameObject* wolf1 = 0;
GameObject* wolf2 = 0;

// Global image variables
int World1Img = -1;
int World2Img = -1;
int player1Img = -1;
int player2Img = -1;
int imgT, imgL1, imgL2, imgL3, imgL4, imgR1, imgR2, imgR3, imgR4;
int mini1bg, mini2bg;

// Game initials
int gameState = 0;
bool game_completion = false;
int completionTimeSeconds = -1;
int screenWidth = 1920, screenHeight = 980;
double speed = 4.0;
int offset = 5;

//individual screen
int screenW = 920, screenH = 650;
int questBarH = 90;
//screen player1
int screen1X = 20, screen1Y = 20;
//screen p2
int screen2X = 980, screen2Y = 20;

// Separate map indicator ranges for each player viewport
int left = 1;   // P1 starts on Map 1
int right = 10; // P2 starts on Map 10

// Snapshot of the initialized gameplay data.
// Used to start a fresh session without reloading/recreating assets.
static GameObject initialGameObjects[MAX_OBJECTS];
static int initialObjectCount = 0;
static int initialInventory[MAX_ITEM_TYPES] = { 0 };


void captureInitialGameState()
{
    initialObjectCount = objectCount;
    std::memcpy(initialGameObjects, objects, sizeof(objects));
    std::memcpy(initialInventory, globalInventory, sizeof(globalInventory));
}

void resetGameSession()
{
    objectCount = initialObjectCount;
    std::memcpy(objects, initialGameObjects, sizeof(objects));
    std::memcpy(globalInventory, initialInventory, sizeof(globalInventory));

    // These values belong to a new gameplay session.
    left = 1;
    right = 10;

	game_completion = false;
    completionTimeSeconds = -1;
    Leaderboard::resetSessionSaveState();
    Leaderboard::savePendingPlayerInfo(
        PlayerData::teamName,
        PlayerData::player1Name,
        PlayerData::player2Name
    );

    // Do not carry movement-key states from the abandoned session into
    // the newly started one.
    for (int i = 0; i < 512; ++i)
    {
        keyPressed[i] = 0;
        specialKeyPressed[i] = 0;
    }

    // Object addresses remain the same after copying the snapshot.
    player1 = &objects[0];
    player2 = &objects[1];

    wolf1 = loadedCompanionP1;
    wolf2 = loadedCompanionP2;

    PlayerAnimation::reset();

	// Reset entrance & ruin completion flags
    resetEntranceState();

}

void mapLoader1(){
    //left screen
    if(left == 1){
        iShowImage(screen1X+offset, screen1Y+offset, screenW-2*offset, screenH-2*offset, imgL1);
    }else if(left == 2){
        iShowImage(screen1X+offset, screen1Y+offset, screenW-2*offset, screenH-2*offset, imgL2);
    }else if(left == 3){
        iShowImage(screen1X+offset, screen1Y+offset, screenW-2*offset, screenH-2*offset, imgL3);
    }else if(left == 4){
        iShowImage(screen1X+offset, screen1Y+offset, screenW-2*offset, screenH-2*offset, imgL4);
    }
	// Interior Ruin maps for P1
	else if (left == 101) {
        iShowImage(screen1X+offset, screen1Y+offset, screenW-2*offset, screenH-2*offset, mini1bg);
    }else if (left == 102) {
        iShowImage(screen1X+offset, screen1Y+offset, screenW-2*offset, screenH-2*offset, mini1bg);
    }else if (left == 103) {
        iShowImage(screen1X+offset, screen1Y+offset, screenW-2*offset, screenH-2*offset, mini1bg);
    }
}

void mapLoader2(){
    //right screen
    if(right == 10){
        iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, imgR1);
    }else if(right == 11){
        iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, imgR2);
    }else if(right == 12){
        iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, imgR3);
    }else if(right == 13){
        iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, imgR4);
    }	
	// Interior Ruin maps for P2
	else if (right == 104) {
        iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, mini2bg);
    }else if (right == 105) {
        iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, mini2bg);
    }else if (right == 106) {
        iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, mini2bg);
    }
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

    }else if(gameState == 1 || gameState == PauseMenu::PAUSED_STATE){

        // --- PLAYER 1 VIEWPORT ---

        iSetColor(168, 169, 173);
        iFilledRectangle(screen1X, screen1Y, screenW, screenH);
        mapLoader1();

        iSetColor(168, 169, 173);
        iFilledRectangle(screen1X, screen1Y+screenH, screenW, questBarH);
        iShowImage(screen1X+offset, screen1Y+screenH, screenW-2*offset, questBarH-offset, World1Img);

        GameObject* bgListP1[MAX_OBJECTS];
        int bgCountP1 = 0;
        getBackgroundDrawListForMap(left, bgListP1, &bgCountP1);
        for (int i = 0; i < bgCountP1; i++) {
            if (bgListP1[i]->id == OBJ_EFFECT && !bgListP1[i]->isActivated) continue;
            if (bgListP1[i]->imgIndex != -1) {
                iShowImage(bgListP1[i]->x, bgListP1[i]->y, bgListP1[i]->width, bgListP1[i]->height, bgListP1[i]->imgIndex);
            }
        }

        GameObject* drawListP1[MAX_OBJECTS];
        int countP1 = 0;
        getSortedDrawListForMap(left, drawListP1, &countP1);
        for (int i = 0; i < countP1; i++) {
            if (drawListP1[i]->imgIndex != -1) {
                iShowImage(drawListP1[i]->x, drawListP1[i]->y, drawListP1[i]->width, drawListP1[i]->height, drawListP1[i]->imgIndex);
            }
        }

        // --- PLAYER 2 VIEWPORT ---

        iSetColor(168, 169, 173);
        iFilledRectangle(screen2X, screen2Y, screenW, screenH);
        mapLoader2();

        iSetColor(168, 169, 173);
        iFilledRectangle(screen2X, screen2Y+screenH, screenW, questBarH);
        iShowImage(screen2X+offset, screen2Y+screenH, screenW-2*offset, questBarH-offset, World2Img);

        GameObject* bgListP2[MAX_OBJECTS];
        int bgCountP2 = 0;
        getBackgroundDrawListForMap(right, bgListP2, &bgCountP2);
        for (int i = 0; i < bgCountP2; i++) {
            if (bgListP2[i]->id == OBJ_EFFECT && !bgListP2[i]->isActivated) continue;
            if (bgListP2[i]->imgIndex != -1) {
                iShowImage(bgListP2[i]->x, bgListP2[i]->y, bgListP2[i]->width, bgListP2[i]->height, bgListP2[i]->imgIndex);
            }
        }

        GameObject* drawListP2[MAX_OBJECTS];
        int countP2 = 0;
        getSortedDrawListForMap(right, drawListP2, &countP2);
        for (int i = 0; i < countP2; i++) {
            if (drawListP2[i]->imgIndex != -1) {
                iShowImage(drawListP2[i]->x, drawListP2[i]->y, drawListP2[i]->width, drawListP2[i]->height, drawListP2[i]->imgIndex);
            }
        }


        CollectablesCount::draw();

        // The pause button is visible only during active gameplay.
        if (gameState == DoitoProhorMenu::PLAY_STATE)
        {
            PauseMenu::drawPauseButton();
        }

        if (gameState == PauseMenu::PAUSED_STATE)
        {
            // Keep the gameplay scene visible and place the pause overlay on top.
            PauseMenu::draw(screenWidth, screenHeight);
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

        if (gameState == PauseMenu::PAUSED_STATE)
        {
            PauseMenu::handleClick(gameState, mx, my);
            return;
        }

        if (gameState == DoitoProhorMenu::PLAY_STATE)
        {
            if (PauseMenu::handleGameplayClick(gameState, mx, my))
            {
                return;
            }
        }

        if (DoitoProhorMenu::isMenuState(gameState))
        {
            int previousState = gameState;
            DoitoProhorMenu::handleClick(gameState, mx, my);

            // Starting from the Player Data screen always starts a fresh gameplay session.
            if (previousState == DoitoProhorMenu::PLAYER_DATA_STATE &&
                gameState == DoitoProhorMenu::PLAY_STATE)
            {
                resetGameSession();
            }

            return;
        }

    }


    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){

    }
}

void fixedUpdate() {
    // Player names are typed inside the iGraphics window while the Player Data
    // screen is active.
    if (gameState == DoitoProhorMenu::PLAYER_DATA_STATE)
    {
        if (PlayerData::updateInput())
        {
            gameState = DoitoProhorMenu::PLAY_STATE;
            resetGameSession();
        }
        return;
    }

    // A pause freezes all gameplay updates, including keyboard-driven movement
    // and animation.
	if (gameState != DoitoProhorMenu::PLAY_STATE) return;

	// ==========================================
    // PLAYER 1 MOVEMENT & MAP TRANSITIONS
    // ==========================================
    if(player1 != 0){
		// Define Player 1 screen boundaries (Viewport 1)
        double p1MinX = screen1X + offset;
        double p1MaxX = screen1X + screenW - offset - player1->width;
        double p1MinY = screen1Y + offset;
        double p1MaxY = screen1Y + screenH - offset - player1->height;

        double dx = 0, dy = 0;
        if (isKeyPressed('d')) dx += 1.0;
        if (isKeyPressed('a')) dx -= 1.0;
        if (isKeyPressed('w')) dy += 1.0;
        if (isKeyPressed('s')) dy -= 1.0;

        if (dx != 0 && dy != 0) { dx *= 0.7071; dy *= 0.7071; }

        double nextX = player1->x + (dx * speed);
        double nextY = player1->y + (dy * speed);

        PlayerAnimation::updatePlayer1(player1);

        // 2x2 Grid Map Transitions for Player 1
        bool mapChanged = false;
        if (left == 1) {
            if (nextY > p1MaxY) { left = 2; player1->y = p1MinY; mapChanged = true; }
            else if (nextX < p1MinX) { left = 3; player1->x = p1MaxX; mapChanged = true; }
        }
        else if (left == 2) {
            if (nextY < p1MinY) { left = 1; player1->y = p1MaxY; mapChanged = true; }
            else if (nextX < p1MinX) { left = 4; player1->x = p1MaxX; mapChanged = true; }
        }
        else if (left == 3) {
            if (nextY > p1MaxY) { left = 4; player1->y = p1MinY; mapChanged = true; }
            else if (nextX > p1MaxX) { left = 1; player1->x = p1MinX; mapChanged = true; }
        }
        else if (left == 4) {
            if (nextY < p1MinY) { left = 3; player1->y = p1MaxY; mapChanged = true; }
            else if (nextX > p1MaxX) { left = 2; player1->x = p1MinX; mapChanged = true; }
        }

        if (mapChanged) {
            player1->mapID = left;
            if (wolf1 != 0 && wolf1->isTamed) {
                wolf1->mapID = left;
                wolf1->x = player1->x - 30;
                wolf1->y = player1->y;
            }
        } 
        else {
            if (nextX >= p1MinX && nextX <= p1MaxX && nextY >= p1MinY && nextY <= p1MaxY) {
                if (!checkCollisionForMap(player1, nextX, nextY, left)) {
                    player1->x = nextX;
                    player1->y = nextY;
                    handleCollectibleCollisionsForMap(player1, left);
                }
            }
        }

        // Remote Switch Trigger ('e')
        if (isKeyPressed('e')){
            triggerRemoteEffectForMap(player1, 25.0, left);
			handleEntranceAndExit(player1, left, true);
        } 

        // Companion Guide Trigger ('q')
        if (wolf1 != 0 && wolf1->isTamed && isKeyPressed('q')) {
            double distX = (player1->x - wolf1->x);
            double distY = (player1->y - wolf1->y);
            if ((distX * distX + distY * distY) <= 4000.0) {
                if (globalInventory[1] >= 1 && !wolf1->isGuiding) {
                    globalInventory[1] -= 1;
                    wolf1->isGuiding = true;
                }
            }
        }
    }

    // ==========================================
    // PLAYER 2 MOVEMENT & MAP TRANSITIONS
    // ==========================================
    if(player2 != 0){
		// Define Player 2 screen boundaries (Viewport 2)
        double p2MinX = screen2X + offset;
        double p2MaxX = screen2X + screenW - offset - player2->width;
        double p2MinY = screen2Y + offset;
        double p2MaxY = screen2Y + screenH - offset - player2->height;

        double dx = 0, dy = 0;
        if (isSpecialKeyPressed(GLUT_KEY_RIGHT)) dx += 1.0;
        if (isSpecialKeyPressed(GLUT_KEY_LEFT)) dx -= 1.0;
        if (isSpecialKeyPressed(GLUT_KEY_UP)) dy += 1.0;
        if (isSpecialKeyPressed(GLUT_KEY_DOWN)) dy -= 1.0;

        if (dx != 0 && dy != 0) { dx *= 0.7071; dy *= 0.7071; }

        double nextX = player2->x + (dx * speed);
        double nextY = player2->y + (dy * speed);

        PlayerAnimation::updatePlayer2(player2);

        // 2x2 Grid Map Transitions for Player 2
        bool mapChanged = false;
        if (right == 10) {
            if (nextY > p2MaxY) { right = 11; player2->y = p2MinY; mapChanged = true; }
            else if (nextX > p2MaxX) { right = 12; player2->x = p2MinX; mapChanged = true; }
        }
        else if (right == 11) {
            if (nextY < p2MinY) { right = 10; player2->y = p2MaxY; mapChanged = true; }
            else if (nextX > p2MaxX) { right = 13; player2->x = p2MinX; mapChanged = true; }
        }
        else if (right == 12) {
            if (nextY > p2MaxY) { right = 13; player2->y = p2MinY; mapChanged = true; }
            else if (nextX < p2MinX) { right = 10; player2->x = p2MaxX; mapChanged = true; }
        }
        else if (right == 13) {
            if (nextY < p2MinY) { right = 12; player2->y = p2MaxY; mapChanged = true; }
            else if (nextX < p2MinX) { right = 11; player2->x = p2MaxX; mapChanged = true; }
        }

        if (mapChanged) {
            player2->mapID = right;
            if (wolf2 != 0 && wolf2->isTamed) {
                wolf2->mapID = right;
                wolf2->x = player2->x - 30;
                wolf2->y = player2->y;
            }
        } 
        else {
            if (nextX >= p2MinX && nextX <= p2MaxX && nextY >= p2MinY && nextY <= p2MaxY) {
                if (!checkCollisionForMap(player2, nextX, nextY, right)) {
                    player2->x = nextX;
                    player2->y = nextY;
                    handleCollectibleCollisionsForMap(player2, right);
                }
            }
        }

        // Remote Switch Trigger ('0')
        if (isKeyPressed('0')){
            triggerRemoteEffectForMap(player2, 25.0, right);
			handleEntranceAndExit(player2, right, true);
        }

        // Companion Guide Trigger ('1')
        if (wolf2 != 0 && wolf2->isTamed && isKeyPressed('1')) {
            double distX = (player2->x - wolf2->x);
            double distY = (player2->y - wolf2->y);
            if ((distX * distX + distY * distY) <= 4000.0) {
                if (globalInventory[1] >= 1 && !wolf2->isGuiding) {
                    globalInventory[1] -= 1;
                    wolf2->isGuiding = true;
                }
            }
        }
    }
}


// This function runs automatically every time the timer fires
void gameLoopUpdate() {
    // FUTURE GAME COMPLETION LOGIC:
    // Set game_completion = true and provide the real elapsed time through
    // completionTimeSeconds when the actual game-winning condition is implemented.
    Leaderboard::saveIfCompleted(
        game_completion,
        completionTimeSeconds,
        PlayerData::teamName,
        PlayerData::player1Name,
        PlayerData::player2Name
    );

	// Companion updates must stop while the game is paused.
    if (gameState != DoitoProhorMenu::PLAY_STATE)
    {
        return;
    }

    updateCompanion(wolf1, player1, 3.0, left);
    updateCompanion(wolf2, player2, 3.0, right);

	updateSpecialInteractions(); // Checks boat, pond, backup switch unlock, and message bottle continuously
}


int main(){
    iInitialize(screenWidth, screenHeight, "Doito Prohor : The Time that Steals");

    // Timer functions
    iSetTimer(10, gameLoopUpdate);


    DoitoProhorMenu::initialize();
    PauseMenu::initialize();
    PlayerAnimation::initialize();
    CollectablesCount::initialize();

    // Image
    // Title
    imgT = iLoadImage("Image//Title.png");
    //player 1 & 2 worlds
    World1Img = iLoadImage("Image//Luptaronyo.png");
    World2Img = iLoadImage("Image//Niharika.png");
    //initialize images of maps
    imgL1 = iLoadImage("Image//bg1world1.png"); // player 1: 2-2 = bottom-right
    imgL2 = iLoadImage("Image//bg1world1.png"); // player 1: 1-2 = top-right
	imgL3 = iLoadImage("Image//bg1world1.png"); // player 1: 2-1 = bottom-left
    imgL4 = iLoadImage("Image//bg1world1.png"); // player 1: 1-1 = top-left
    imgR1 = iLoadImage("Image//bg12world2.png"); // player 2: 2-1 = bottom-left
	imgR2 = iLoadImage("Image//bg12world2.png"); // player 2: 1-1 = top-left
	imgR3 = iLoadImage("Image//bg3world2.png"); // player 2: 2-2 = bottom-right
	imgR4 = iLoadImage("Image//bg4world2.png"); // player 2: 1-2 = top-right

	mini1bg = iLoadImage("Image//mini1bg.png");
	mini2bg = iLoadImage("Image//mini2bg.jpg");
    
    //initialize images of player 1 & 2
    player1Img = iLoadImage("Image//P1F.png");
    player2Img = iLoadImage("Image//P2F.png");


    // Players
    player1 = createObject(OBJ_PLAYER, 400, 75, player1Img, 34, 58, 30, 12, -1);
    player1->mapID = 1;

    player2 = createObject(OBJ_PLAYER, 1440, 30, player2Img, 34, 58, 30, 12, -1);
    player2->mapID = 10;


    loadAllGameMaps();
    wolf1 = loadedCompanionP1;
    wolf2 = loadedCompanionP2;

    // Save the original gameplay snapshot for future Main Menu -> Play resets.
    captureInitialGameState();

    iStart();
    return 0;
}

# define STB_IMAGE_IMPLEMENTATION
# include "stb_image.h"
