#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

#include "GameObject.hpp"


// 3-Argument version with custom touch margin
inline bool checkCollision(GameObject* a, GameObject* b, double margin) {
    if (!a || !b) return false;
    if (a->isHidden || b->isHidden) return false;
    if (a->mapID != b->mapID) return false;

    double aW = (a->collisionWidth > 0) ? a->collisionWidth : ((a->width > 0) ? a->width : 32.0);
    double aH = (a->collisionHeight > 0) ? a->collisionHeight : ((a->height > 0) ? a->height : 32.0);
    double aFullW = (a->width > 0) ? a->width : aW;

    double bW = (b->collisionWidth > 0) ? b->collisionWidth : ((b->width > 0) ? b->width : 32.0);
    double bH = (b->collisionHeight > 0) ? b->collisionHeight : ((b->height > 0) ? b->height : 32.0);
    double bFullW = (b->width > 0) ? b->width : bW;

    double aLeft   = a->x + (aFullW - aW) / 2.0;
    double aRight  = aLeft + aW;
    double aBottom = a->y;
    double aTop    = a->y + aH;

    double bLeft   = b->x + (bFullW - bW) / 2.0;
    double bRight  = bLeft + bW;
    double bBottom = b->y;
    double bTop    = b->y + bH;

    return (aLeft <= bRight + margin && 
            aRight >= bLeft - margin &&
            aBottom <= bTop + margin && 
            aTop >= bBottom - margin);
}

// 2-Argument default overload (calls 3-arg version with 4.0 margin)
inline bool checkCollision(GameObject* a, GameObject* b) {
    return checkCollision(a, b, 4.0);
}


// Checks if a given bounding box collides with any other solid object in the world
inline bool checkCollisionForMap(GameObject* self, double targetX, double targetY, int activeMapID) {
    // Center-based X bounds for 'self'
    double selfColWidth = (self->collisionWidth > 0 ? self->collisionWidth : self->width);
    double selfCenterX = targetX + (self->width / 2.0);
    double selfLeft = selfCenterX - (selfColWidth / 2.0);
    double selfRight = selfCenterX + (selfColWidth / 2.0);

    double selfBottom = targetY;
    double selfTop = targetY + (self->collisionHeight > 0 ? self->collisionHeight : self->height);

    GameObject* allObjects = getAllObjects();
    int count = getObjectCount();

    for (int i = 0; i < count; i++) {
        GameObject* other = &allObjects[i];

		// Do not check collision against itself
        if (other == self) continue;

		// Ignore objects on other maps
		if (other->mapID != activeMapID) continue;

		// Ignore switches that are hidden
		if (other->id == OBJ_SWITCH && other->isHidden) continue;

		// If the other object has no collision height, skip it
        if (other->collisionHeight <= 0) continue;

		// If it is an effect object & it is not active, skip it
        if (other->id == OBJ_EFFECT) {
            if (!other->isActivated) {
                continue; 
            }
        }

		// Calculate the other object's collision box (from base Y up to collisionHeight)
        // Center-based X bounds for 'other'
        double otherColWidth = (other->collisionWidth > 0 ? other->collisionWidth : other->width);
        double otherCenterX = other->x + (other->width / 2.0);
        double otherLeft = otherCenterX - (otherColWidth / 2.0);
        double otherRight = otherCenterX + (otherColWidth / 2.0);

        double otherBottom = other->y;
        double otherTop = other->y + other->collisionHeight;

		// Standard AABB Collision Check
        if (selfLeft < otherRight && selfRight > otherLeft &&
            selfBottom < otherTop && selfTop > otherBottom) {
            return true; // Collision detected 
        }
    }
    return false; // No collision 
}


// Collectible collision handler mapping to item type inventory
inline void handleCollectibleCollisionsForMap(GameObject* player, int activeMapID) {
    if (player == 0) return;

    double pColWidth = (player->collisionWidth > 0 ? player->collisionWidth : player->width);
    double pCenterX = player->x + (player->width / 2.0);
    double pLeft = pCenterX - (pColWidth / 2.0);
    double pRight = pCenterX + (pColWidth / 2.0);
    double pBottom = player->y;
    double pTop = player->y + (player->collisionHeight > 0 ? player->collisionHeight : player->height);

    GameObject* allObjects = getAllObjects();
    int count = getObjectCount();

    for (int i = 0; i < count; i++) {
        GameObject* obj = &allObjects[i];

		// Ignore items on other maps
		if (obj->mapID != activeMapID) continue;

        if (obj->id == OBJ_COLLECTIBLE && !obj->isCollected) {
            double cLeft = obj->x;
            double cRight = obj->x + obj->width;
            double cBottom = obj->y;
            double cTop = obj->y + obj->height;

            if (pLeft < cRight && pRight > cLeft &&
                pBottom < cTop && pTop > cBottom) {
                obj->isCollected = true;
                
                // Add to the specific item type slot in global shared inventory
                int type = obj->itemType;
                if (type >= 0 && type < MAX_ITEM_TYPES) {
                    globalInventory[type]++;
                }
            }
        }
    }
}

#endif