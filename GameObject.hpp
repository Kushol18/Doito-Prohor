#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP


// Define types of objects
enum ObjectType {
    OBJ_TREE,
    OBJ_PLAYER,
    OBJ_ENEMY
};

// GameObject structure
struct GameObject {
    int id;
    double x, y;
	int imgIndex;
    double width, height;
	double collisionHeight;
};


#endif
