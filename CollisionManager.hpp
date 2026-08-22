#ifndef COLLISION_MANAGER_HPP
#ifndef DRAW_MANAGER_HPP // Ensure DrawManager functions are accessible
#include "DrawManager.hpp"
#endif

#define COLLISION_MANAGER_HPP

// Checks if a given bounding box collides with any other solid object in the world
inline bool checkCollision(GameObject* self, double targetX, double targetY) {
    double selfLeft = targetX;
    double selfRight = targetX + self->width;
    double selfBottom = targetY;
    double selfTop = targetY + (self->collisionHeight > 0 ? self->collisionHeight : self->height);

    GameObject* allObjects = getAllObjects();
    int count = getObjectCount();

    for (int i = 0; i < count; i++) {
        GameObject* other = &allObjects[i];

        // Do not check collision against itself
        if (other == self) continue;

        // If the other object has no collision height, skip it
        if (other->collisionHeight <= 0) continue;

		// If it is an effect object & it is not active, skip it
		if (other->id == OBJ_EFFECT && !other->isActivated) continue;

        // Calculate the other object's collision box (from base Y up to collisionHeight)
        double otherLeft = other->x;
        double otherRight = other->x + other->width;
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

#endif
