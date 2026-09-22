#ifndef ENTRANCE_MANAGER_HPP
#define ENTRANCE_MANAGER_HPP

#include "GameObject.hpp"
#include "Collectables_Count.hpp"

// Inventory Item Indices
const int ITEM_TIMESHARD = 0; 
const int ITEM_FOOD = 1;
const int ITEM_HOURGLASS_FRAGMENT = 2; 

static bool isRuinCompleted[7] = { false };

inline void resetEntranceState() {
    for (int i = 0; i < 7; i++) {
        isRuinCompleted[i] = false;
    }
}

inline void handleEntranceAndExit(GameObject* player, int& currentMapID, bool interactionPressed) {
    if (!player) return;

    GameObject* allObjects = getAllObjects();
    int count = getObjectCount();

    // 1. Enter Ruin Logic
    if (interactionPressed && currentMapID >= 1 && currentMapID <= 13) {
        for (int i = 0; i < count; i++) {
            if (allObjects[i].mapID == currentMapID && allObjects[i].id == OBJ_ENTRANCE) {
                double dx = (player->x + player->width / 2.0) - (allObjects[i].x + allObjects[i].width / 2.0);
                double dy = (player->y + player->height / 2.0) - (allObjects[i].y + allObjects[i].height / 2.0);

                if ((dx * dx + dy * dy) <= 3600.0) { 
                    int variant = allObjects[i].miniGameVariant;

                    if (variant >= 1 && variant <= 6 && isRuinCompleted[variant]) {
                        return; 
                    }

                    currentMapID = 100 + variant;
                    player->mapID = currentMapID;

                    double tileW = 880.0 / 15.0; // ~58.66px
                    double tileH = 600.0 / 11.0; // ~54.54px

                    if (currentMapID == 101) { // Player 1 Entrance
                        player->x = 40.0 + (0 * tileW) + (tileW - player->width) / 2.0;
                        player->y = 40.0 + (1 * tileH) + (tileH - player->height) / 2.0;
                    } 
                    else if (currentMapID == 104) { // Player 2 Entrance
                        player->x = 1000.0 + (0 * tileW) + (tileW - player->width) / 2.0;
                        player->y = 40.0 + (1 * tileH) + (tileH - player->height) / 2.0;
                    }
                    return;
                }
            }
        }
    }

    // 2. Exit Ruin Trigger & Reward Distribution
    else if (currentMapID >= 101 && currentMapID <= 106) {
        int ruinVariant = currentMapID - 100;
        bool reachedExit = false;

        double tileW = 880.0 / 15.0;
        double tileH = 600.0 / 11.0;

        if (currentMapID == 101) {
            double exitX = 40.0 + (14 * tileW);
            double exitY = 40.0 + (9 * tileH);
            if (player->x >= exitX - 10.0 && player->y >= exitY - 10.0) {
                reachedExit = true;
            }
        }
        else if (currentMapID == 104) {
            double exitX = 1000.0 + (14 * tileW);
            double exitY = 40.0 + (9 * tileH);
            if (player->x >= exitX - 10.0 && player->y >= exitY - 10.0) {
                reachedExit = true;
            }
        }

        if (reachedExit) {
            if (!isRuinCompleted[ruinVariant]) {
                isRuinCompleted[ruinVariant] = true;

                // --- REWARD LOGIC ---
                // Variant 1 & 4 (First mini-game of each world) -> 2 Timeshards
                // Variants 2, 3, 5, 6 (Other mini-games) -> 6 Timeshards
                int timeshardsToAward = (ruinVariant == 1 || ruinVariant == 4) ? 2 : 6;

                globalInventory[ITEM_TIMESHARD] += timeshardsToAward;         // Index 0
                globalInventory[ITEM_HOURGLASS_FRAGMENT] += 1;                 // Index 2
            }

            int returnMap = 1;
            double returnX = 400, returnY = 200;

            if (ruinVariant == 1)      { returnMap = 1;  returnX = 675;  returnY = 180; }
            else if (ruinVariant == 4) { returnMap = 10; returnX = 1625; returnY = 180; }

            currentMapID = returnMap;
            player->mapID = currentMapID;
            player->x = returnX;
            player->y = returnY;
        }
    }
}

#endif