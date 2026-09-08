#ifndef COMPANION_MANAGER_HPP
#define COMPANION_MANAGER_HPP

#include "GameObject.hpp"
#include "CollisionManager.hpp"
#include <cmath>

inline void updateCompanion(GameObject* companion, GameObject* player, double speed) {
    if (companion == 0 || player == 0) return;
    if (companion->id != OBJ_COMPANION) return; 

    // Taming logic if companion is untamed
    if (!companion->isTamed) {
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

    // Active follower and hint behavior
    GameObject* allObjects = getAllObjects();
    int count = getObjectCount();

    double targetX = player->x;
    double targetY = player->y;
    
    double hintDetectionRadius = 250.0; 
    double stopDistance = 35.0;         

    for (int i = 0; i < count; i++) {
        if (allObjects[i].id == OBJ_SWITCH) {
            GameObject* sw = &allObjects[i];
            
            // Ignore switches that have already been triggered/paid for
            if (sw->isCostPaid) continue;

            double sdx = sw->x - companion->x;
            double sdy = sw->y - companion->y;
            double distSq = (sdx * sdx) + (sdy * sdy);

            if (distSq <= (hintDetectionRadius * hintDetectionRadius)) {
                targetX = sw->x;
                targetY = sw->y;
                break; 
            }
        }
    }

    double dx = targetX - companion->x;
    double dy = targetY - companion->y;
    double distance = std::sqrt(dx * dx + dy * dy);

    if (distance > stopDistance) {
        double vx = (dx / distance) * speed;
        double vy = (dy / distance) * speed;
        
        double nextX = companion->x + vx;
        double nextY = companion->y + vy;

        if (!checkCollision(companion, nextX, nextY)) {
            companion->x = nextX;
            companion->y = nextY;
        } 
        else {
            if (!checkCollision(companion, nextX, companion->y)) {
                companion->x = nextX;
            } 
            else if (!checkCollision(companion, companion->x, nextY)) {
                companion->y = nextY;
            } 
            else {
                double perpX = -vy * 0.7;
                double perpY = vx * 0.7;
                if (!checkCollision(companion, companion->x + perpX, companion->y + perpY)) {
                    companion->x += perpX;
                    companion->y += perpY;
                }
            }
        }
    }
}

#endif