#ifndef TEMPORAL_COUPLING_HPP
#define TEMPORAL_COUPLING_HPP

#include "GameObject.hpp"


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
                // If the cost hasn't been paid yet, enforce item-specific requirements
                if (!switchObj->isCostPaid) {
                    int reqType = switchObj->requiredItemType;
                    int reqAmt = switchObj->requiredAmount;

                    if (reqType >= 0 && reqType < MAX_ITEM_TYPES && globalInventory[reqType] >= reqAmt) {
                        globalInventory[reqType] -= reqAmt; // Deduct the specific item type
                        switchObj->isCostPaid = true;         // Mark cost paid forever
                    } else {
                        return; // Not enough of the required item type
                    }
                }

                // Toggle target object normally once cost is satisfied
                int targetIdx = switchObj->targetIndex;
                if (targetIdx >= 0 && targetIdx < count) {
                    GameObject* targetObj = &allObjects[targetIdx];
                    targetObj->isActivated = true;
                }
            }
        }
    }
}

#endif
