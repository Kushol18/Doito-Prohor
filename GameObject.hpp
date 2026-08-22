#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP


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


#endif
