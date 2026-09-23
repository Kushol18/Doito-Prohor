#ifndef ENTRANCE_MANAGER_HPP
#define ENTRANCE_MANAGER_HPP

#include "GameObject.hpp"
#include "Collectables_Count.hpp"
#include "GameEnd.hpp"
#include "Audios.hpp"

// Inventory Item Indices
const int ITEM_TIMESHARD = 0; 
const int ITEM_FOOD = 1;
const int ITEM_HOURGLASS_FRAGMENT = 2; 

bool isRuinCompleted[7] = { false };

inline void resetEntranceState() {
    for (int i = 0; i < 7; i++) {
        isRuinCompleted[i] = false;
    }
}

inline void handleEntranceAndExit(GameObject* player, int& currentMapID, bool interactionPressed) {
    if (!player) return;

    GameObject* allObjects = getAllObjects();
    int count = getObjectCount();

    // 1. Enter a structure maze. The existing entrance objects remain the
    // authoritative source of which variant is entered.
    if (interactionPressed && currentMapID >= 1 && currentMapID <= 13) {
        for (int i = 0; i < count; i++) {
            if (allObjects[i].mapID == currentMapID && allObjects[i].id == OBJ_ENTRANCE) {
                double dx = (player->x + player->width / 2.0) -
                            (allObjects[i].x + allObjects[i].width / 2.0);
                double dy = (player->y + player->height / 2.0) -
                            (allObjects[i].y + allObjects[i].height / 2.0);

                if ((dx * dx + dy * dy) <= 3600.0) {
                    int variant = allObjects[i].miniGameVariant;
                    if (variant < 1 || variant > 6 || isRuinCompleted[variant]) return;

                    // Use the existing entrance configuration from MapLoader.
                    // ST1 and ST4 have a required amount of 0; ST2/ST3/ST5/ST6
                    // are configured to require 5 Time Shards.
                    const int requiredItemType = allObjects[i].requiredItemType;
                    const int requiredTimeShards = allObjects[i].requiredAmount;

                    if (requiredItemType != ITEM_TIMESHARD ||
                        requiredTimeShards < 0 ||
                        globalInventory[ITEM_TIMESHARD] < requiredTimeShards) {
                        return;
                    }

                    // Pay the entrance cost exactly once, at the moment the
                    // player successfully enters the maze.
                    if (requiredTimeShards > 0) {
                        globalInventory[ITEM_TIMESHARD] -= requiredTimeShards;
                    }

                    currentMapID = 100 + variant;
                    player->mapID = currentMapID;

                    const double tileW = 880.0 / 15.0;
                    const double tileH = 600.0 / 11.0;
                    const double xOff = (variant >= 4) ? 1000.0 : 40.0;

                    player->x = xOff + (tileW - player->width) / 2.0;
                    player->y = 40.0 + (1 * tileH) + (tileH - player->height) / 2.0;

                    Audios::playEntrance();
                    return;
                }
            }
        }
        return;
    }

    // 2. Exit a completed maze, award its hourglass/time rewards once, then
    // return the player to the corresponding overworld map.
    if (currentMapID < 101 || currentMapID > 106) return;

    const int ruinVariant = currentMapID - 100;
    const double tileW = 880.0 / 15.0;
    const double tileH = 600.0 / 11.0;
    const double xOff = (ruinVariant >= 4) ? 1000.0 : 40.0;
    const double exitX = xOff + (14 * tileW);
    const double exitY = 40.0 + (9 * tileH);

    if (!(player->x >= exitX - 10.0 && player->y >= exitY - 10.0)) return;

    if (!isRuinCompleted[ruinVariant]) {
        isRuinCompleted[ruinVariant] = true;

        // Preserve the project's existing time-shard reward quantities.
        const int timeshardsToAward =
            (ruinVariant == 1 || ruinVariant == 4) ? 2 : 6;
        globalInventory[ITEM_TIMESHARD] += timeshardsToAward;

        // New requirement: exactly one hourglass shard for each of the six
        // structures, kept in the existing shared inventory slot.
        globalInventory[ITEM_HOURGLASS_FRAGMENT] += 1;
        Audios::playCollectible();
    }

    int returnMap = 1;
    double returnX = 400.0;
    double returnY = 200.0;

    switch (ruinVariant) {
    case 1: returnMap = 1;  returnX = 675.0; returnY = 180.0; break;
    case 2: returnMap = 2;  returnX = 235.0; returnY = 180.0; break;
    case 3: returnMap = 3;  returnX = 235.0; returnY = 160.0; break;
    case 4: returnMap = 10; returnX = 1625.0; returnY = 180.0; break;
    case 5: returnMap = 11; returnX = 1205.0; returnY = 180.0; break;
    case 6: returnMap = 12; returnX = 1235.0; returnY = 160.0; break;
    default: break;
    }

    currentMapID = returnMap;
    player->mapID = currentMapID;
    player->x = returnX;
    player->y = returnY;
}

#endif