#ifndef TEMPORAL_COUPLING_HPP
#ifndef DRAW_MANAGER_HPP
#include "DrawManager.hpp"
#endif

#define TEMPORAL_COUPLING_HPP

inline void triggerRemoteEffect(GameObject* player, double interactionDistance) {
    GameObject* allObjects = getAllObjects();
    int count = getObjectCount();

    for (int i = 0; i < count; i++) {
        // Find switch objects
        if (allObjects[i].id == OBJ_SWITCH) {
            GameObject* switchObj = &allObjects[i];

            // Check distance between player and this specific switch
            double distX = (player->x + player->width / 2.0) - (switchObj->x + switchObj->width / 2.0);
            double distY = (player->y + player->height / 2.0) - (switchObj->y + switchObj->height / 2.0);
            double distance = (distX * distX) + (distY * distY);

            // Check if squared distance is less than or equal to the squared interaction distance
            if (distance <= (interactionDistance * interactionDistance)) {
                // Check if this switch has a valid target assigned
                int targetIdx = switchObj->targetIndex;
                if (targetIdx >= 0 && targetIdx < count) {
                    GameObject* targetObj = &allObjects[targetIdx];

                    // Set the state of the specific paired target object to true
                    targetObj->isActivated = true;
                }
            }
        }
    }
}

#endif
