#ifndef MAP_LOADER_HPP
#define MAP_LOADER_HPP

#include "GameObject.hpp"
#include "iGraphics.h"

static GameObject* loadedCompanionP1 = 0;
static GameObject* loadedCompanionP2 = 0;

inline void loadAllGameMaps() {
    int timeShardImg = iLoadImage("Image//timeshard.png");
    int foodImg = iLoadImage("Image//food.png");
	int tree = iLoadImage("Image//tree1.png");
	int ruin = iLoadImage("Image//st1.png");
    int effect1Img = iLoadImage("Image//bridge1.png");
    int switch1Img = iLoadImage("Image//platform1.png");
    int wolfImg = iLoadImage("Image//CoWolf.png");

    // ==========================================
    // PLAYER 1 MAPS (ID Range: 1 to 9)
    // ==========================================
    
    // --- P1 Map 1 (mapID = 1) ---
    // timeshard
	GameObject* p1_shard1 = createObject(OBJ_COLLECTIBLE, 100, 100, timeShardImg, 20, 20, 0, 0, -1);
    p1_shard1->itemType = 0;
    p1_shard1->mapID = 1;
	// food
	GameObject* food = createObject(OBJ_COLLECTIBLE, 300, 300, foodImg, 20, 20, 0, 0, -1);
	food->itemType = 1;
	food->mapID = 1;
	// tree
	GameObject* p1_tree1 = createObject(OBJ_TREE, 200, 100, tree, 150, 150, 20, 15, -1);
	p1_tree1->mapID = 1;
	GameObject* p1_tree2 = createObject(OBJ_TREE, 300, 500, tree, 150, 150, 20, 15, -1);
	p1_tree2->mapID = 1;
	GameObject* p1_tree3 = createObject(OBJ_TREE, 500, 100, tree, 150, 150, 20, 15, -1);
	p1_tree3->mapID = 1;

	// ruins
	GameObject* p1_ruin1 = createObject(OBJ_OBSTACLES, 500, 200, ruin, 400, 400, 400, 350, -1);
	p1_ruin1->mapID = 1;
	
	// effect
    GameObject* p1_effect1 = createObject(OBJ_EFFECT, 400, 500, effect1Img, 220, 60, 0, 0, -1);
    p1_effect1->mapID = 1;
	// switch
    GameObject* p1_switch1 = createObject(OBJ_SWITCH, 150, 100, switch1Img, 30, 30, 0, 0, getObjectIndex(p1_effect1));
    p1_switch1->requiredItemType = 0;
    p1_switch1->requiredAmount = 1;
    p1_switch1->mapID = 1;
	// companion
    loadedCompanionP1 = createObject(OBJ_COMPANION, 300, 400, wolfImg, 20, 40, 18, 20, -1);
    loadedCompanionP1->isTamed = false;
    loadedCompanionP1->requiredItemType = 1;
    loadedCompanionP1->requiredAmount = 1;
    loadedCompanionP1->mapID = 1;

    // --- P1 Map 2 (mapID = 2) ---
    GameObject* p1_shard2 = createObject(OBJ_COLLECTIBLE, 200, 200, timeShardImg, 20, 20, 0, 0, -1);
    p1_shard2->itemType = 0;
    p1_shard2->mapID = 2;

	// tree
	GameObject* p1_tree21 = createObject(OBJ_TREE, 300, 100, tree, 150, 150, 20, 15, -1);
	p1_tree21->mapID = 2;
	GameObject* p1_tree22 = createObject(OBJ_TREE, 300, 200, tree, 150, 150, 20, 15, -1);
	p1_tree22->mapID = 2;
	GameObject* p1_tree23 = createObject(OBJ_TREE, 500, 300, tree, 150, 150, 20, 15, -1);
	p1_tree23->mapID = 2;


    // ==========================================
    // PLAYER 2 MAPS (ID Range: 10 to 19)
    // ==========================================

    // --- P2 Map 1 (mapID = 10) ---
    GameObject* p2_shard1 = createObject(OBJ_COLLECTIBLE, 1200, 300, timeShardImg, 20, 20, 0, 0, -1);
    p2_shard1->itemType = 0;
    p2_shard1->mapID = 10;

    GameObject* p2_effect1 = createObject(OBJ_EFFECT, 1200, 500, effect1Img, 220, 60, 0, 0, -1);
    p2_effect1->mapID = 10;

    GameObject* p2_switch1 = createObject(OBJ_SWITCH, 1200, 150, switch1Img, 30, 30, 0, 0, getObjectIndex(p2_effect1));
    p2_switch1->requiredItemType = 0;
    p2_switch1->requiredAmount = 1;
    p2_switch1->mapID = 10;

    loadedCompanionP2 = createObject(OBJ_COMPANION, 1400, 450, wolfImg, 20, 40, 18, 20, -1);
    loadedCompanionP2->isTamed = false;
    loadedCompanionP2->requiredItemType = 1;
    loadedCompanionP2->requiredAmount = 1;
    loadedCompanionP2->mapID = 10;

    // --- P2 Map 2 (mapID = 11) ---
    GameObject* p2_shard2 = createObject(OBJ_COLLECTIBLE, 1400, 400, timeShardImg, 20, 20, 0, 0, -1);
    p2_shard2->itemType = 0;
    p2_shard2->mapID = 11;
}

#endif