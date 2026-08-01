#ifndef DRAW_MANAGER_HPP
#define DRAW_MANAGER_HPP

#include "GameObject.hpp"

#define MAX_OBJECTS 100

static GameObject objects[MAX_OBJECTS];
static int objectCount = 0;

// Creates an object and returns its direct memory address
inline GameObject* createObject(ObjectType type, double x, double y, int imgIndex, int width, int height) {
    if (objectCount < MAX_OBJECTS) {
        objects[objectCount].id = type;
        objects[objectCount].x = x;
        objects[objectCount].y = y;
        objects[objectCount].imgIndex = imgIndex;
        objects[objectCount].width = width;
        objects[objectCount].height = height;
        
        int currentIndex = objectCount;
        objectCount++;
        return &objects[currentIndex]; 
    }
    return 0;
}

// Fills an external array with sorted pointers based strictly on the Y coordinate
inline void getSortedDrawList(GameObject* sortedList[], int* count) {
    for (int i = 0; i < objectCount; i++) {
        sortedList[i] = &objects[i];
    }

    // Sort pointers based strictly on Y coordinates
    for (int i = 1; i < objectCount; i++) {
        GameObject* key = sortedList[i];
        int j = i - 1;
        
        while (j >= 0 && sortedList[j]->y < key->y) {
            sortedList[j + 1] = sortedList[j];
            j = j - 1;
        }
        sortedList[j + 1] = key;
    }
    *count = objectCount;
}

#endif