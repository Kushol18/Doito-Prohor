#ifndef COMPANION_MANAGER_HPP
#define COMPANION_MANAGER_HPP

#include "GameObject.hpp"
#include "CollisionManager.hpp"
#include <cmath>

inline void updateCompanion(GameObject* companion, GameObject* player, double speed, int activeMapID) {
    if (companion == 0 || player == 0) return;
    if (companion->id != OBJ_COMPANION) return; 

    // Taming logic if companion is untamed
    if (!companion->isTamed) {
		if (companion->mapID != activeMapID) return;

        double dx = (player->x + player->width / 2.0) - (companion->x + companion->width / 2.0);
        double dy = (player->y + player->height / 2.0) - (companion->y + companion->height / 2.0);
        double distanceSq = (dx * dx) + (dy * dy);
        double interactionDist = 60.0;

        if (distanceSq <= (interactionDist * interactionDist)) {
            int reqType = companion->requiredItemType;
            int reqAmt = companion->requiredAmount;

            if (globalInventory[reqType] >= reqAmt) {
                globalInventory[reqType] -= reqAmt;
                companion->isTamed = true;
            }
        }
        return; 
    }

	// Sync companion map if following player across maps
    if (companion->mapID != activeMapID) {
        companion->mapID = activeMapID;
    }

    // Determine target position: default to following the player
    double targetX = player->x;
    double targetY = player->y;
    double stopDistance = 35.0;         

    // ONLY hunt for switches if the player has paid/activated Guide Mode
    if (companion->isGuiding) {
        GameObject* allObjects = getAllObjects();
        int count = getObjectCount();
        double hintDetectionRadius = 500.0; // Larger range when actively searching

        for (int i = 0; i < count; i++) {
            GameObject* sw = &allObjects[i];
            if (sw->mapID != activeMapID) continue;

            if (sw->id == OBJ_SWITCH) {
                // SKIP if the switch cost is already paid OR if the switch is hidden!
                if (sw->isCostPaid || sw->isHidden) continue;

                double sdx = sw->x - companion->x;
                double sdy = sw->y - companion->y;
                double distSq = (sdx * sdx) + (sdy * sdy);

                if (distSq <= (hintDetectionRadius * hintDetectionRadius)) {
                    targetX = sw->x;
                    targetY = sw->y;
                    
                    // If companion reaches the switch, turn off guiding mode
                    double reachDist = (companion->x - sw->x) * (companion->x - sw->x) + (companion->y - sw->y) * (companion->y - sw->y);
                    if (reachDist < 400.0) { // Within 20 pixels
                        companion->isGuiding = false;
                    }
                    break; 
                }
            }
        }
    }

    // Movement towards target (Player or Switch)
    double dx = targetX - companion->x;
    double dy = targetY - companion->y;
    double distance = std::sqrt(dx * dx + dy * dy);

    if (distance > stopDistance) {
        double vx = (dx / distance) * speed;
        double vy = (dy / distance) * speed;
        
        double nextX = companion->x + vx;
        double nextY = companion->y + vy;

        if (!checkCollisionForMap(companion, nextX, nextY, activeMapID)) {
            companion->x = nextX;
            companion->y = nextY;
        } 
        else {
            if (!checkCollisionForMap(companion, nextX, companion->y, activeMapID)) {
                companion->x = nextX;
            } 
            else if (!checkCollisionForMap(companion, companion->x, nextY, activeMapID)) {
                companion->y = nextY;
            }
        }
    }
}

#endif