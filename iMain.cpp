#include "iGraphics.h"
#include "DrawManager.hpp"
#include "CollisionManager.hpp"
#include "TemporalCoupling.hpp"
#include "MainMenu.hpp"
#include "PauseMenu.hpp"
#include "PlayerAnimation.hpp"
#include "Collectables_Count.hpp"
#include "CompanionManager.hpp"
#include "MapLoader.hpp"
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
int imgT, imgL1, imgL2, imgR1, imgR2;

// Game initials
int gameState = 0;
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
}

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
    if(right == 10){
        iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, imgR1);
    }else if(right == 11){
        iShowImage(screen2X+offset, screen2Y+offset, screenW-2*offset, screenH-2*offset, imgR2);
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

            // A new Play selection always starts from the original gameplay state.
            if (previousState == DoitoProhorMenu::MAIN_MENU_STATE &&
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
    // A pause freezes all gameplay updates, including keyboard-driven movement
    // and animation.
    if (gameState != DoitoProhorMenu::PLAY_STATE)
    {
        return;
    }

    // Keys allocated for player 1
    if(player1 != 0){
        double nextX = player1->x;
        double nextY = player1->y;

        if ( isKeyPressed('d')){
            nextX += speed;
        } // Player 1 right
        if (isKeyPressed('w')){
            nextY += speed;
        } // Player 1 up
        if (isKeyPressed('a')){
            nextX -= speed;
        } // Player 1 left
        if (isKeyPressed('s')){
            nextY -= speed;
        } // Player 1 down

        PlayerAnimation::updatePlayer1(player1);

        if (!checkCollisionForMap(player1, nextX, nextY, left)) {
            player1->x = nextX;
            player1->y = nextY;
            handleCollectibleCollisionsForMap(player1, left); // Checks and updates items
        } // If no collision then update the x & y values

        if (isKeyPressed('e')){
            triggerRemoteEffectForMap(player1, 25.0, left);
        } // Trigger the OBJ_EFFECT for the OBJ_SWITCH on the coordinate of player 1

        if (isKeyPressed('q')){
            left = 2;
        } // changes map

    }

    // Keys allocated for player 2
    if(player2 != 0){
        double nextX = player2->x;
        double nextY = player2->y;

        if (isSpecialKeyPressed(GLUT_KEY_RIGHT)){
            nextX += speed;
        } // Player 2 right
        if (isSpecialKeyPressed(GLUT_KEY_UP)){
            nextY += speed;
        } // Player 2 up
        if (isSpecialKeyPressed(GLUT_KEY_LEFT)){
            nextX -= speed;
        } // Player 2 left
        if (isSpecialKeyPressed(GLUT_KEY_DOWN)){
            nextY -= speed;
        } // Player 2 down

        PlayerAnimation::updatePlayer2(player2);

        if (!checkCollisionForMap(player2, nextX, nextY, right)) {
            player2->x = nextX;
            player2->y = nextY;
            handleCollectibleCollisionsForMap(player2, right); // Checks and updates items
        }
        // If no collision then update the x & y values
        if (isKeyPressed('0')){
            triggerRemoteEffectForMap(player2, 25.0, right);
        } // Trigger the OBJ_EFFECT for the OBJ_SWITCH on the coordinate of player 2

    }



    if (isKeyPressed(' ')){
        // Playing the audio once
        //mciSendString("play ggsong from 0", NULL, 0, NULL);
    }
}


// This function runs automatically every time the timer fires
void gameLoopUpdate() {
    // Companion updates must stop while the game is paused.
    if (gameState != DoitoProhorMenu::PLAY_STATE)
    {
        return;
    }

    updateCompanion(wolf1, player1, 3.0, left);
    updateCompanion(wolf2, player2, 3.0, right);
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
    imgL1 = iLoadImage("Image//bgworld1.png");
    imgL2 = iLoadImage("Image//bgworld1.png");
    imgR1 = iLoadImage("Image//bgworld2.png");
    //imgR2 = iLoadImage("Image//bgworld2.png");
    //initialize images of player 1 & 2
    player1Img = iLoadImage("Image//P1F.png");
    player2Img = iLoadImage("Image//P2F.png");


    // Players
    player1 = createObject(OBJ_PLAYER, 460, 30, player1Img, 24, 48, 20, 10, -1);
    player1->mapID = 1;

    player2 = createObject(OBJ_PLAYER, 1440, 30, player2Img, 24, 48, 20, 10, -1);
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
