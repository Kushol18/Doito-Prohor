#ifndef PLAYER_WEAPON_HPP
#define PLAYER_WEAPON_HPP

#include "iGraphics.h"
#include "GameObject.hpp"
#include <cstring>

namespace PlayerWeapon
{
    enum Direction
    {
        UP = 0,
        DOWN = 1,
        LEFT = 2,
        RIGHT = 3
    };

    static const int FRAME_DELAY = 6;
    static const int ATTACK_FRAME_DELAY = 5;

    static int p1Frames[4][3];
    static int p2Frames[4][3];
    static int p1AttackLeft[3];
    static int p1AttackRight[3];
    static int p2AttackLeft[2];
    static int p2AttackRight[3];

    static Direction p1Direction = DOWN;
    static Direction p2Direction = DOWN;
    static int p1Frame = 0;
    static int p2Frame = 0;
    static int p1Counter = 0;
    static int p2Counter = 0;
    static int p1AttackFrame = 0;
    static int p2AttackFrame = 0;
    static int p1AttackCounter = 0;
    static int p2AttackCounter = 0;
    static bool p1Attacking = false;
    static bool p2Attacking = false;
    static bool prevP1AttackKey = false;
    static bool prevP2AttackKey = false;
    static bool p1AttackRequested = false;
    static bool p2AttackRequested = false;
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

        p1Frames[UP][0]    = loadImage("Image//rp1b1.png");
        p1Frames[UP][1]    = loadImage("Image//rp1b2.png");
        p1Frames[UP][2]    = loadImage("Image//rp1b3.png");
        p1Frames[DOWN][0]  = loadImage("Image//rp1f1.png");
        p1Frames[DOWN][1]  = loadImage("Image//rp1f2.png");
        p1Frames[DOWN][2]  = loadImage("Image//rp1f3.png");
        p1Frames[LEFT][0]  = loadImage("Image//rp1l1.png");
        p1Frames[LEFT][1]  = loadImage("Image//rp1l2.png");
        p1Frames[LEFT][2]  = loadImage("Image//rp1l3.png");
        p1Frames[RIGHT][0] = loadImage("Image//rp1r1.png");
        p1Frames[RIGHT][1] = loadImage("Image//rp1r2.png");
        p1Frames[RIGHT][2] = loadImage("Image//rp1r3.png");

        p2Frames[UP][0]    = loadImage("Image//rp2b1.png");
        p2Frames[UP][1]    = loadImage("Image//rp2b2.png");
        p2Frames[UP][2]    = loadImage("Image//rp2b3.png");
        p2Frames[DOWN][0]  = loadImage("Image//rp2f1.png");
        p2Frames[DOWN][1]  = loadImage("Image//rp2f2.png");
        p2Frames[DOWN][2]  = loadImage("Image//rp2f3.png");
        p2Frames[LEFT][0]  = loadImage("Image//rp2l1.png");
        p2Frames[LEFT][1]  = loadImage("Image//rp2l2.png");
        p2Frames[LEFT][2]  = loadImage("Image//rp2l3.png");
        // The uploaded project has rp2r2/rp2r3/rp2r4, with no rp2r1.
        p2Frames[RIGHT][0] = loadImage("Image//rp2r2.png");
        p2Frames[RIGHT][1] = loadImage("Image//rp2r3.png");
        p2Frames[RIGHT][2] = loadImage("Image//rp2r4.png");

        p1AttackLeft[0] = loadImage("Image//rp1la1.png");
        p1AttackLeft[1] = loadImage("Image//rp1la2.png");
        p1AttackLeft[2] = loadImage("Image//rp1la3.png");
        p1AttackRight[0] = loadImage("Image//rp1ra1.png");
        p1AttackRight[1] = loadImage("Image//rp1ra2.png");
        p1AttackRight[2] = loadImage("Image//rp1ra3.png");

        // Uploaded P2 left attack set has two frames: rp2la2/rp2la3.
        p2AttackLeft[0] = loadImage("Image//rp2la2.png");
        p2AttackLeft[1] = loadImage("Image//rp2la3.png");
        p2AttackRight[0] = loadImage("Image//rp2ra1.png");
        p2AttackRight[1] = loadImage("Image//rp2ra2.png");
        p2AttackRight[2] = loadImage("Image//rp2ra3.png");

        imagesLoaded = true;
    }

    inline void reset()
    {
        p1Direction = DOWN;
        p2Direction = DOWN;
        p1Frame = p2Frame = 0;
        p1Counter = p2Counter = 0;
        p1AttackFrame = p2AttackFrame = 0;
        p1AttackCounter = p2AttackCounter = 0;
        p1Attacking = p2Attacking = false;
        prevP1AttackKey = prevP2AttackKey = false;
        p1AttackRequested = p2AttackRequested = false;
    }

    inline void setIdleOrMoveSprite(GameObject* player, bool playerOne)
    {
        if (!player) return;

        bool moving = false;
        if (playerOne)
        {
            if (isKeyPressed('d')) { p1Direction = RIGHT; moving = true; }
            if (isKeyPressed('w')) { p1Direction = UP; moving = true; }
            if (isKeyPressed('a')) { p1Direction = LEFT; moving = true; }
            if (isKeyPressed('s')) { p1Direction = DOWN; moving = true; }

            if (!moving) p1Counter = 0;
            else if (++p1Counter >= FRAME_DELAY)
            {
                p1Counter = 0;
                p1Frame = (p1Frame + 1) % 3;
            }
            player->imgIndex = p1Frames[p1Direction][moving ? p1Frame : 0];
        }
        else
        {
            if (isSpecialKeyPressed(GLUT_KEY_RIGHT)) { p2Direction = RIGHT; moving = true; }
            if (isSpecialKeyPressed(GLUT_KEY_UP)) { p2Direction = UP; moving = true; }
            if (isSpecialKeyPressed(GLUT_KEY_LEFT)) { p2Direction = LEFT; moving = true; }
            if (isSpecialKeyPressed(GLUT_KEY_DOWN)) { p2Direction = DOWN; moving = true; }

            if (!moving) p2Counter = 0;
            else if (++p2Counter >= FRAME_DELAY)
            {
                p2Counter = 0;
                p2Frame = (p2Frame + 1) % 3;
            }
            player->imgIndex = p2Frames[p2Direction][moving ? p2Frame : 0];
        }
    }

    inline void startAttack(bool playerOne)
    {
        if (playerOne)
        {
            if (!p1Attacking) {
                p1Attacking = true;
                p1AttackFrame = 0;
                p1AttackCounter = 0;
                p1AttackRequested = true;
            }
        }
        else
        {
            if (!p2Attacking) {
                p2Attacking = true;
                p2AttackFrame = 0;
                p2AttackCounter = 0;
                p2AttackRequested = true;
            }
        }
    }

    inline void updateAttackSprite(GameObject* player, bool playerOne)
    {
        if (!player) return;

        if (playerOne)
        {
            int* attackFrames = (p1Direction == LEFT) ? p1AttackLeft : p1AttackRight;
            if (p1Direction == UP || p1Direction == DOWN)
            {
                // No vertical attack sprites exist in the uploaded project;
                // use the corresponding horizontal attack based on the last
                // horizontal facing direction available in the set.
                attackFrames = p1AttackRight;
            }

            player->imgIndex = attackFrames[p1AttackFrame];
            if (++p1AttackCounter >= ATTACK_FRAME_DELAY)
            {
                p1AttackCounter = 0;
                ++p1AttackFrame;
                if (p1AttackFrame >= 3)
                {
                    p1Attacking = false;
                    p1AttackFrame = 0;
                }
            }
        }
        else
        {
            if (p2Direction == LEFT)
            {
                int frame = p2AttackFrame;
                if (frame > 1) frame = 1;
                player->imgIndex = p2AttackLeft[frame];
                if (++p2AttackCounter >= ATTACK_FRAME_DELAY)
                {
                    p2AttackCounter = 0;
                    ++p2AttackFrame;
                    if (p2AttackFrame >= 2)
                    {
                        p2Attacking = false;
                        p2AttackFrame = 0;
                    }
                }
            }
            else
            {
                player->imgIndex = p2AttackRight[p2AttackFrame];
                if (++p2AttackCounter >= ATTACK_FRAME_DELAY)
                {
                    p2AttackCounter = 0;
                    ++p2AttackFrame;
                    if (p2AttackFrame >= 3)
                    {
                        p2Attacking = false;
                        p2AttackFrame = 0;
                    }
                }
            }
        }
    }


    inline bool consumeAttackRequest(bool playerOne)
    {
        bool& requested = playerOne ? p1AttackRequested : p2AttackRequested;
        bool value = requested;
        requested = false;
        return value;
    }

    inline void update(GameObject* player, bool playerOne)
    {
        if (!player || !player->hasSword) return;

        bool attackNow = playerOne ? (isKeyPressed('f') != 0) : (isKeyPressed('5') != 0);
        bool& previous = playerOne ? prevP1AttackKey : prevP2AttackKey;

        if (attackNow && !previous && !((playerOne ? p1Attacking : p2Attacking)))
        {
            startAttack(playerOne);
        }
        previous = attackNow;

        if (playerOne ? p1Attacking : p2Attacking)
        {
            updateAttackSprite(player, playerOne);
        }
        else
        {
            setIdleOrMoveSprite(player, playerOne);
        }
    }
}

#endif
