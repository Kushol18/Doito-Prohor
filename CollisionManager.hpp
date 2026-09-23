#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

#include "GameObject.hpp"
#include "Audios.hpp"
#include "GameEnd.hpp"

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

inline bool checkCollision(GameObject* a, GameObject* b) {
    return checkCollision(a, b, 4.0);
}

inline bool checkCollisionForMap(GameObject* self, double targetX, double targetY, int activeMapID) {
    if (!self) return false;

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
        if (other == self) continue;
        if (other->mapID != activeMapID) continue;
        if (other->id == OBJ_SWITCH && other->isHidden) continue;
        if (other->collisionHeight <= 0) continue;
        if (other->id == OBJ_EFFECT && !other->isActivated) continue;
        if (other->id == OBJ_COLLECTIBLE || other->id == OBJ_ENTRANCE || other->id == OBJ_EXIT) continue;

        double otherColWidth = (other->collisionWidth > 0 ? other->collisionWidth : other->width);
        double otherCenterX = other->x + (other->width / 2.0);
        double otherLeft = otherCenterX - (otherColWidth / 2.0);
        double otherRight = otherCenterX + (otherColWidth / 2.0);
        double otherBottom = other->y;
        double otherTop = other->y + other->collisionHeight;

        if (selfLeft < otherRight && selfRight > otherLeft &&
            selfBottom < otherTop && selfTop > otherBottom) {
            return true;
        }
    }
    return false;
}

inline void handleCollectibleCollisionsForMap(GameObject* player, int activeMapID) {
    if (!player) return;

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
        if (obj->mapID != activeMapID) continue;
        if (obj->id != OBJ_COLLECTIBLE || obj->isCollected) continue;

        double cLeft = obj->x;
        double cRight = obj->x + obj->width;
        double cBottom = obj->y;
        double cTop = obj->y + obj->height;

        if (pLeft < cRight && pRight > cLeft && pBottom < cTop && pTop > cBottom) {
            obj->isCollected = true;

            if (obj->itemType == 4)
            {
                // Sword ownership is player-specific, unlike the shared Bandage.
                player->hasSword = true;
            }
            else
            {
                int type = obj->itemType;
                if (type >= 0 && type < MAX_ITEM_TYPES)
                    globalInventory[type]++;
            }

            Audios::playCollectible();
            return;
        }
    }
}

#endif
