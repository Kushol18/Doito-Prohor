#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP


#define MAX_OBJECTS 100


// Define types of objects
enum ObjectType {
    OBJ_TREE,
    OBJ_PLAYER,
    OBJ_ENEMY,
	OBJ_SWITCH,
	OBJ_EFFECT
};

// GameObject structure
struct GameObject {
    int id;
    double x, y;
	int imgIndex;
    double width, height;
	double collisionHeight;
	bool isActivated; // For all objects except OBJ_EFFECT this value can only be false, but OBJ_EFFECT can be assigned true as well	
	int targetIndex; // For all objects except OBJ_SWITCH this value can only be -1, but for OBJ_SWITCH: it stores the exact index of the specific OBJ_EFFECT it controls
};


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


#endif
