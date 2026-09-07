#ifndef DRAW_MANAGER_HPP
#define DRAW_MANAGER_HPP

#include "GameObject.hpp"


// Gathers only switches and effects for rendering in the background
inline void getBackgroundDrawList(GameObject* bgList[], int* count) {
	*count = 0;
	GameObject* allObjs = getAllObjects();
	int totalCount = getObjectCount();

	for (int i = 0; i < totalCount; i++) {
		if (allObjs[i].id == OBJ_SWITCH || allObjs[i].id == OBJ_EFFECT) {
			bgList[*count] = &allObjs[i];
			(*count)++;
		}
	}
}

// Fills an external array with sorted pointers based strictly on the Y coordinate (except switches and effects)
inline void getSortedDrawList(GameObject* sortedList[], int* count) {
    *count = 0;
	GameObject* allObjs = getAllObjects();
	int totalCount = getObjectCount();

    GameObject* temp[MAX_OBJECTS];
    int tempCount = 0;

    for (int i = 0; i < totalCount; i++) {
        if (allObjs[i].id != OBJ_SWITCH && allObjs[i].id != OBJ_EFFECT) {
            temp[tempCount] = &allObjs[i];
            tempCount++;
        }
    }

    for (int i = 0; i < tempCount; i++) {
        sortedList[i] = temp[i];
    } // sortedList is holding the coppied values not yet sorted

    for (int i = 1; i < tempCount; i++) {
        GameObject* key = sortedList[i];
        int j = i - 1;
        
        while (j >= 0 && sortedList[j]->y < key->y) {
            sortedList[j + 1] = sortedList[j];
            j = j - 1;
        }
        sortedList[j + 1] = key;
    }
    *count = tempCount;
}

#endif