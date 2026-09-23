#ifndef PET_ANIMATION_HPP
#define PET_ANIMATION_HPP

#include "iGraphics.h"
#include "GameObject.hpp"
#include <cmath>

namespace PetAnimation
{
    enum Direction
    {
        UP = 0,
        DOWN = 1,
        LEFT = 2,
        RIGHT = 3
    };

    static const int FRAME_DELAY = 7;
    static int p1Frames[4][4];
    static int p2Frames[4][4];
    static int p1Frame = 0;
    static int p2Frame = 0;
    static int p1Counter = 0;
    static int p2Counter = 0;
    static Direction p1Direction = DOWN;
    static Direction p2Direction = DOWN;
    static double p1LastX = 0.0, p1LastY = 0.0;
    static double p2LastX = 0.0, p2LastY = 0.0;
    static bool imagesLoaded = false;

    inline int loadImage(const char* path)
    {
        char buffer[128];
        std::strcpy(buffer, path);
        return (int)iLoadImage(buffer);
    }

    inline void initialize()
    {
        if (imagesLoaded) return;

        p1Frames[UP][0] = loadImage("Image//p1p1b1.png");
        p1Frames[UP][1] = loadImage("Image//p1p1b2.png");
        p1Frames[UP][2] = loadImage("Image//p1p1b3.png");
        p1Frames[UP][3] = loadImage("Image//p1p1b4.png");
        p1Frames[DOWN][0] = loadImage("Image//p1p1f1.png");
        p1Frames[DOWN][1] = loadImage("Image//p1p1f2.png");
        p1Frames[DOWN][2] = loadImage("Image//p1p1f3.png");
        p1Frames[DOWN][3] = loadImage("Image//p1p1f4.png");
        p1Frames[LEFT][0] = loadImage("Image//p1p1l1.png");
        p1Frames[LEFT][1] = loadImage("Image//p1p1l2.png");
        p1Frames[LEFT][2] = loadImage("Image//p1p1l3.png");
        p1Frames[LEFT][3] = loadImage("Image//p1p1l4.png");
        p1Frames[RIGHT][0] = loadImage("Image//p1p1r1.png");
        p1Frames[RIGHT][1] = loadImage("Image//p1p1r2.png");
        p1Frames[RIGHT][2] = loadImage("Image//p1p1r13.png");
        p1Frames[RIGHT][3] = loadImage("Image//p1p1r4.png");

        p2Frames[UP][0] = loadImage("Image//p2p2b1.png");
        p2Frames[UP][1] = loadImage("Image//p2p2b2.png");
        p2Frames[UP][2] = loadImage("Image//p2p2b3.png");
        p2Frames[UP][3] = loadImage("Image//p2p2b4.png");
        p2Frames[DOWN][0] = loadImage("Image//p2p2f1.png");
        p2Frames[DOWN][1] = loadImage("Image//p2p2f2.png");
        p2Frames[DOWN][2] = loadImage("Image//p2p2f3.png");
        p2Frames[DOWN][3] = loadImage("Image//p2p2f4.png");
        p2Frames[LEFT][0] = loadImage("Image//p2p2l1.png");
        p2Frames[LEFT][1] = loadImage("Image//p2p2l2.png");
        p2Frames[LEFT][2] = loadImage("Image//p2p2l3.png");
        p2Frames[LEFT][3] = loadImage("Image//p2p2l4.png");
        p2Frames[RIGHT][0] = loadImage("Image//p2p2r1.png");
        p2Frames[RIGHT][1] = loadImage("Image//p2p2r2.png");
        p2Frames[RIGHT][2] = loadImage("Image//p2p2r3.png");
        p2Frames[RIGHT][3] = loadImage("Image//p2p2r4.png");

        imagesLoaded = true;
    }

    inline void reset(GameObject* pet1, GameObject* pet2)
    {
        p1Frame = p2Frame = 0;
        p1Counter = p2Counter = 0;
        p1Direction = p2Direction = DOWN;
        p1LastX = pet1 ? pet1->x : 0.0;
        p1LastY = pet1 ? pet1->y : 0.0;
        p2LastX = pet2 ? pet2->x : 0.0;
        p2LastY = pet2 ? pet2->y : 0.0;

        if (pet1) pet1->imgIndex = p1Frames[p1Direction][0];
        if (pet2) pet2->imgIndex = p2Frames[p2Direction][0];
    }

    inline Direction directionFromDelta(double dx, double dy, Direction fallback)
    {
        const double EPS = 0.01;
        if (std::fabs(dx) < EPS && std::fabs(dy) < EPS) return fallback;
        if (std::fabs(dx) >= std::fabs(dy)) return dx > 0.0 ? RIGHT : LEFT;
        return dy > 0.0 ? UP : DOWN;
    }

    inline void updateOne(GameObject* pet, bool playerOne)
    {
        if (!pet || pet->id != OBJ_COMPANION) return;

        if (playerOne)
        {
            double dx = pet->x - p1LastX;
            double dy = pet->y - p1LastY;
            p1Direction = directionFromDelta(dx, dy, p1Direction);
            p1LastX = pet->x;
            p1LastY = pet->y;

            if (std::fabs(dx) < 0.01 && std::fabs(dy) < 0.01)
            {
                p1Counter = 0;
                pet->imgIndex = p1Frames[p1Direction][p1Frame];
                return;
            }

            if (++p1Counter >= FRAME_DELAY)
            {
                p1Counter = 0;
                p1Frame = (p1Frame + 1) % 4;
            }
            pet->imgIndex = p1Frames[p1Direction][p1Frame];
        }
        else
        {
            double dx = pet->x - p2LastX;
            double dy = pet->y - p2LastY;
            p2Direction = directionFromDelta(dx, dy, p2Direction);
            p2LastX = pet->x;
            p2LastY = pet->y;

            if (std::fabs(dx) < 0.01 && std::fabs(dy) < 0.01)
            {
                p2Counter = 0;
                pet->imgIndex = p2Frames[p2Direction][p2Frame];
                return;
            }

            if (++p2Counter >= FRAME_DELAY)
            {
                p2Counter = 0;
                p2Frame = (p2Frame + 1) % 4;
            }
            pet->imgIndex = p2Frames[p2Direction][p2Frame];
        }
    }

    inline void update(GameObject* pet1, GameObject* pet2)
    {
        updateOne(pet1, true);
        updateOne(pet2, false);
    }
}

#endif
