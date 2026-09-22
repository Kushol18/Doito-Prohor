#ifndef MAP_LOADER_HPP
#define MAP_LOADER_HPP

#include "GameObject.hpp"
#include "iGraphics.h"

static GameObject* loadedCompanionP1 = 0;
static GameObject* loadedCompanionP2 = 0;


inline void spawnNaturalClustersForMap(
    int mapID, 
    int treeImg, int bushImg, 
    double treeW, double treeH, double treeColW, double treeColH, 
    double bushW, double bushH, double bushColW, double bushColH, 
    int numTrees, int numBushes, 
    double mapWidth, double mapHeight, 
    double offsetX
) {
    double margin = 25.0;

    double minTreeX = margin, maxTreeX = mapWidth - treeW - margin;
    double minTreeY = margin, maxTreeY = mapHeight - treeH - margin;
    
    double minBushX = margin, maxBushX = mapWidth - bushW - margin;
    double minBushY = margin, maxBushY = mapHeight - bushH - margin;

    auto canPlaceSafely = [&](double renderX, double renderY, double renderW, double renderH, double colW, double colH, double minSpacing) -> bool {
        
        double candColY = renderY; 
        double candColH = colH;

        GameObject* allObjs = getAllObjects();
        int totalCount = getObjectCount();

        for (int i = 0; i < totalCount; i++) {
            if (allObjs[i].mapID != mapID) continue;

            double ex = allObjs[i].x;
            double ey = allObjs[i].y;
            double ew = allObjs[i].width;   
            double eh = allObjs[i].height;  

            // --- 1. Obstacle Collision Check ---
            if (allObjs[i].id == OBJ_OBSTACLES) {
                double obsColW = allObjs[i].collisionWidth;
                double obsColH = allObjs[i].collisionHeight;
                
                double obsColX = ex + (ew - obsColW) / 2.0;
                double obsColY = ey; 

                // THE FIX: 
                // X-Axis uses FULL RENDER BOUNDS to prevent branches/image bleeding into sides
                bool intersectX = (renderX < obsColX + obsColW) && (renderX + renderW > obsColX);
                
                // Y-Axis uses COLLISION BOUNDS so it can still spawn in front/behind the obstacle
                bool intersectY = (candColY < obsColY + obsColH) && (candColY + candColH > obsColY);

                if (intersectX && intersectY) {
                    return false; // Safely blocked!
                }
            }

            // --- 2. Anti-Clumping Check ---
            if (allObjs[i].id == OBJ_TREE) {
                double otherCenterX = ex + (ew / 2.0);
                double otherCenterY = ey + (eh / 2.0);
                double candCenterX = renderX + (renderW / 2.0);
                double candCenterY = renderY + (renderH / 2.0);

                double dx = candCenterX - otherCenterX;
                double dy = candCenterY - otherCenterY;
                
                if ((dx * dx) + (dy * dy) < (minSpacing * minSpacing)) {
                    return false; 
                }
            }
        }
        return true;
    };

    // --- Spawn Trees ---
    for (int i = 0; i < numTrees; i++) {
        int attempts = 0;
        while (attempts++ < 100) {
            double rx = minTreeX + (rand() % (int)(maxTreeX - minTreeX + 1));
            double ry = minTreeY + (rand() % (int)(maxTreeY - minTreeY + 1));

            if (canPlaceSafely(rx + offsetX, ry, treeW, treeH, treeColW, treeColH, 120.0)) {
                GameObject* tree = createObject(OBJ_TREE, rx + offsetX, ry, treeImg, treeW, treeH, treeColW, treeColH, -1);
                tree->mapID = mapID;
                break;
            }
        }
    }

    // --- Spawn Bushes ---
    for (int i = 0; i < numBushes; i++) {
        int attempts = 0;
        while (attempts++ < 100) {
            double rx = minBushX + (rand() % (int)(maxBushX - minBushX + 1));
            double ry = minBushY + (rand() % (int)(maxBushY - minBushY + 1));

            if (canPlaceSafely(rx + offsetX, ry, bushW, bushH, bushColW, bushColH, 80.0)) {
                GameObject* bush = createObject(OBJ_TREE, rx + offsetX, ry, bushImg, bushW, bushH, bushColW, bushColH, -1);
                bush->mapID = mapID;
                break;
            }
        }
    }
}


inline void loadAllGameMaps() {
    int timeShardImg = iLoadImage("Image//timeshard.png");
    int foodImg = iLoadImage("Image//food.png");
	int tree = iLoadImage("Image//tree.png");
	int bush = iLoadImage("Image//bush.png");
	int cactus = iLoadImage("Image//cactus.png");
	int minicactus = iLoadImage("Image//minicactus.png");
	int st1 = iLoadImage("Image//st1.png");
	int st2 = iLoadImage("Image//st2.png");
	int st3 = iLoadImage("Image//st3.png");
	int st4 = iLoadImage("Image//st4.png");
	int st5 = iLoadImage("Image//st5.png");
	int st6 = iLoadImage("Image//st6.png");
	int portal1 = iLoadImage("Image//portal1.png");
	int portal2 = iLoadImage("Image//portal2.png");
	int pond1 = iLoadImage("Image//pond1.jpg");
	int pond2 = iLoadImage("Image//pond2.jpg");
	int entranceImg = iLoadImage("Image//entrance.png");
    int effect1Img = iLoadImage("Image//bridge1.png");
    int sw1 = iLoadImage("Image//sw1.png");
	int sw2 = iLoadImage("Image//sw2.png");
    int wolfImg = iLoadImage("Image//CoWolf.png");

    // ==========================================
    // PLAYER 1 MAPS (ID Range: 1 to 9)
    // ==========================================
    
    // --- P1 Map 1 (mapID = 1) ---
	// ruins
	GameObject* p1_st1 = createObject(OBJ_OBSTACLES, 500, 265, st1, 400, 400, 400, 250, -1);
	p1_st1->mapID = 1;
	// Creating an entrance object for Structure 1
	GameObject* entrance1 = createObject(OBJ_ENTRANCE, 675, 240, entranceImg, 64, 32, 0, 0, -1);
	entrance1->mapID = 1;
	entrance1->miniGameVariant = 1;
    // timeshard
	GameObject* p1_shard1 = createObject(OBJ_COLLECTIBLE, 100, 100, timeShardImg, 20, 20, 0, 0, -1);
    p1_shard1->itemType = 0;
    p1_shard1->mapID = 1;
	// food
	GameObject* food1 = createObject(OBJ_COLLECTIBLE, 300, 300, foodImg, 20, 20, 0, 0, -1);
	food1->itemType = 1;
	food1->mapID = 1;
	GameObject* food2 = createObject(OBJ_COLLECTIBLE, 200, 250, foodImg, 20, 20, 0, 0, -1);
	food2->itemType = 1;
	food2->mapID = 1;
	// effect
    GameObject* p1_effect1 = createObject(OBJ_EFFECT, 400, 500, effect1Img, 220, 60, 0, 0, -1);
    p1_effect1->mapID = 1;
	// switch
    GameObject* p1_switch1 = createObject(OBJ_SWITCH, 150, 100, sw1, 50, 50, 0, 0, getObjectIndex(p1_effect1));
    p1_switch1->requiredItemType = 0;
    p1_switch1->requiredAmount = 1;
    p1_switch1->mapID = 1;
	p1_switch1->isHidden = true;
	// companion
    loadedCompanionP1 = createObject(OBJ_COMPANION, 300, 400, wolfImg, 34, 58, 20, 10, -1);
    loadedCompanionP1->isTamed = false;
    loadedCompanionP1->requiredItemType = 1;
    loadedCompanionP1->requiredAmount = 1;
    loadedCompanionP1->mapID = 1;
	// Nature Elements
	spawnNaturalClustersForMap(1, tree, bush, 160, 160, 20, 15, 40, 40, 0, 0, 11, 14, 920.0, 650.0, 0);
	

    // --- P1 Map 2 (mapID = 2) ---
    GameObject* p1_shard2 = createObject(OBJ_COLLECTIBLE, 200, 200, timeShardImg, 20, 20, 0, 0, -1);
    p1_shard2->itemType = 0;
    p1_shard2->mapID = 2;

	// ruins
	GameObject* p1_st2 = createObject(OBJ_OBSTACLES, 60, 260, st2, 400, 400, 400, 250, -1);
	p1_st2->mapID = 2;
	// Creating an entrance object for Structure 2
	GameObject* entrance2 = createObject(OBJ_ENTRANCE, 235, 235, entranceImg, 64, 32, 0, 0, -1);
	entrance2->mapID = 2;
	entrance2->miniGameVariant = 2;
	// Nature Elements
	spawnNaturalClustersForMap(2, tree, bush, 160, 160, 20, 15, 40, 40, 0, 0, 9, 14, 920.0, 650.0, 0);


	// --- P1 Map 3 (mapID = 3) ---
	// ruins
	GameObject* p1_st3 = createObject(OBJ_OBSTACLES, 60, 240, st3, 400, 400, 400, 250, -1);
	p1_st3->mapID = 3;
	// Creating an entrance object for Structure 3
	GameObject* entrance3 = createObject(OBJ_ENTRANCE, 235, 215, entranceImg, 64, 32, 0, 0, -1);
	entrance3->mapID = 3;
	entrance3->miniGameVariant = 3;
	// Nature Elements
	spawnNaturalClustersForMap(3, tree, bush, 160, 160, 20, 15, 40, 40, 0, 0, 10, 16, 920.0, 650.0, 0);


	// --- P1 Map 4 (mapID = 4) ---
	// ruins
	GameObject* p1_portal = createObject(OBJ_OBSTACLES, 450, 500, portal1, 100, 110, 70, 25, -1);
	p1_portal->mapID = 4;
	// pond
	GameObject* p1_pond = createObject(OBJ_OBSTACLES, 260, 200, pond1, 240, 240, 240, 240, -1);
	p1_pond->mapID = 4;
	// Nature Elements
	spawnNaturalClustersForMap(4, tree, bush, 160, 160, 20, 15, 40, 40, 0, 0, 10, 14, 920.0, 650.0, 0);



    // ==========================================
    // PLAYER 2 MAPS (ID Range: 10 to 19)
    // ==========================================

    // --- P2 Map 10 (mapID = 10) ---
	// ruins
	GameObject* p2_st4 = createObject(OBJ_OBSTACLES, 1450, 265, st4, 400, 400, 400, 250, -1);
	p2_st4->mapID = 10;
	// Creating an entrance object for Structure 4
	GameObject* entrance4 = createObject(OBJ_ENTRANCE, 1625, 240, entranceImg, 64, 32, 0, 0, -1);
	entrance4->mapID = 10;
	entrance4->miniGameVariant = 4;
	// companion
    loadedCompanionP2 = createObject(OBJ_COMPANION, 1200, 400, wolfImg, 34, 58, 20, 10, -1);
    loadedCompanionP2->isTamed = false;
    loadedCompanionP2->requiredItemType = 1;
    loadedCompanionP2->requiredAmount = 1;
    loadedCompanionP2->mapID = 10;
	// Nature Elements
	spawnNaturalClustersForMap(10, cactus, minicactus, 70, 100, 26, 15, 40, 40, 20, 20, 8, 12, 920.0, 650.0, 960.0);
	

    // --- P2 Map 11 (mapID = 11) ---
    //timeshard
	GameObject* p2_shard1 = createObject(OBJ_COLLECTIBLE, 1100, 200, timeShardImg, 20, 20, 0, 0, -1);
    p2_shard1->itemType = 0;
    p2_shard1->mapID = 11;

	// ruins
	GameObject* p2_st5 = createObject(OBJ_OBSTACLES, 1030, 260, st5, 400, 400, 400, 250, -1);
	p2_st5->mapID = 11;
	// Creating an entrance object for Structure 5
	GameObject* entrance5 = createObject(OBJ_ENTRANCE, 1205, 235, entranceImg, 64, 32, 0, 0, -1);
	entrance5->mapID = 11;
	entrance5->miniGameVariant = 5;
	// Nature Elements
	spawnNaturalClustersForMap(11, cactus, minicactus, 70, 100, 26, 15, 40, 40, 20, 20, 7, 13, 920.0, 650.0, 960.0);


	// --- P2 Map 12 (mapID = 12) ---
	// ruins
	GameObject* p2_st6 = createObject(OBJ_OBSTACLES, 1060, 240, st6, 400, 400, 400, 250, -1);
	p2_st6->mapID = 12;
	// Creating an entrance object for Structure 6
	GameObject* entrance6 = createObject(OBJ_ENTRANCE, 1235, 215, entranceImg, 64, 32, 0, 0, -1);
	entrance6->mapID = 12;
	entrance6->miniGameVariant = 6;
	//invisible box
	GameObject* invi1 = createObject(OBJ_OBSTACLES, 1590, 0, entranceImg, 310, 700, 350, 700, -1);
	invi1->mapID = 12;
	// Nature Elements
	spawnNaturalClustersForMap(12, cactus, minicactus, 70, 100, 26, 15, 40, 40, 20, 20, 6, 7, 920.0, 650.0, 960.0);


	// --- P2 Map 13 (mapID = 13) ---
	// ruins
	GameObject* p2_portal = createObject(OBJ_OBSTACLES, 1250, 500, portal2, 100, 110, 70, 25, -1);
	p2_portal->mapID = 13;
	// pond
	GameObject* p2_pond = createObject(OBJ_OBSTACLES, 1200, 200, pond2, 240, 240, 240, 240, -1);
	p2_pond->mapID = 13;
	//invisible box
	GameObject* invi2 = createObject(OBJ_OBSTACLES, 1590, 0, entranceImg, 310, 700, 350, 700, -1);
	invi2->mapID = 13;
	// Nature Elements
	spawnNaturalClustersForMap(13, cactus, minicactus, 70, 100, 26, 15, 40, 40, 20, 20, 7, 6, 920.0, 650.0, 960);

}

#endif