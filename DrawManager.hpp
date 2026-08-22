#ifndef DRAW_MANAGER_HPP
#define DRAW_MANAGER_HPP

#include "GameObject.hpp"

#define MAX_OBJECTS 100

static GameObject objects[MAX_OBJECTS];
static int objectCount = 0;

// Creates an object and returns its direct memory address
inline GameObject* createObject(ObjectType type, double x, double y, int imgIndex, int width, int height, double collisionHeight, int targetIndex) {
    if (objectCount < MAX_OBJECTS) {
        objects[objectCount].id = type;
        objects[objectCount].x = x;
        objects[objectCount].y = y;
        objects[objectCount].imgIndex = imgIndex;
        objects[objectCount].width = width;
        objects[objectCount].height = height;
        objects[objectCount].collisionHeight = collisionHeight;
		objects[objectCount].isActivated = false;
		objects[objectCount].targetIndex = targetIndex;
        
        int currentIndex = objectCount;
        objectCount++;
        return &objects[currentIndex];
    }
    return 0;
}

// Retrieves the total object array reference for collision checks
inline GameObject* getAllObjects() {
    return objects;
}

inline int getObjectCount() {
    return objectCount;
}

// Helper to get an object's index by its memory pointer (needed for switch-effect pairing)
inline int getObjectIndex(GameObject* obj){
	if (obj == 0) return -1; 

	return obj - objects; // Pointer arithmetic gives the exact array index
}

// Gathers only switches and effects for rendering in the background
inline void getBackgroundDrawList(GameObject* bgList[], int* count) {
	*count = 0;
	for (int i = 0; i < objectCount; i++) {
		if (objects[i].id == OBJ_SWITCH || objects[i].id == OBJ_EFFECT) {
			bgList[*count] = &objects[i];
			(*count)++;
		}
	}
}

// Fills an external array with sorted pointers based strictly on the Y coordinate (except switches and effects)
inline void getSortedDrawList(GameObject* sortedList[], int* count) {
    *count = 0;
    GameObject* temp[MAX_OBJECTS];
    int tempCount = 0;

    for (int i = 0; i < objectCount; i++) {
        if (objects[i].id != OBJ_SWITCH && objects[i].id != OBJ_EFFECT) {
            temp[tempCount] = &objects[i];
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