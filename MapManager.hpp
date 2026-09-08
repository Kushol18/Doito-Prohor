#ifndef MAP_MANAGER_HPP
#define MAP_MANAGER_HPP

#include "GameObject.hpp"

inline void transitionPlayerToMap(GameObject* player, GameObject* companion, int newMapID, double newX, double newY) {
    if (player != 0) {
        player->x = newX;
        player->y = newY;
        player->mapID = newMapID;
    }

    if (companion != 0 && companion->isTamed) {
        companion->x = newX - 40;
        companion->y = newY;
        companion->mapID = newMapID;
    }
}

#endif