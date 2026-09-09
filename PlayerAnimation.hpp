#ifndef PLAYER_ANIMATION_HPP
#define PLAYER_ANIMATION_HPP

#include "iGraphics.h"
#include "GameObject.hpp"

namespace PlayerAnimation
{
    // Four movement directions, matching the supplied sprite groups.
    enum Direction
    {
        UP = 0,
        DOWN = 1,
        LEFT = 2,
        RIGHT = 3
    };

    // fixedUpdate() is sampled by iGraphics at the existing keyboard
    // sampling rate (16 ms by default). Six samples gives about 96 ms
    // per animation frame (~10.4 frames/second).
    static const int FRAME_DELAY = 6;

    static int p1Frames[4][4];
    static int p2Frames[4][4];

    static int p1Frame = 0;
    static int p2Frame = 0;
    static int p1Counter = 0;
    static int p2Counter = 0;
    static Direction p1Direction = DOWN;
    static Direction p2Direction = DOWN;
    static bool imagesLoaded = false;

    static void initialize()
    {
        if (imagesLoaded) return;

        // Player 1: W/A/S/D -> Up/Left/Down/Right.
        p1Frames[UP][0]    = iLoadImage("Image//gb1.png");
        p1Frames[UP][1]    = iLoadImage("Image//gb2.png");
        p1Frames[UP][2]    = iLoadImage("Image//gb3.png");
        p1Frames[UP][3]    = iLoadImage("Image//gb4.png");

        p1Frames[DOWN][0]  = iLoadImage("Image//gf1.png");
        p1Frames[DOWN][1]  = iLoadImage("Image//gf2.png");
        p1Frames[DOWN][2]  = iLoadImage("Image//gf3.png");
        p1Frames[DOWN][3]  = iLoadImage("Image//gf4.png");

        p1Frames[LEFT][0]  = iLoadImage("Image//gl1.png");
        p1Frames[LEFT][1]  = iLoadImage("Image//gl2.png");
        p1Frames[LEFT][2]  = iLoadImage("Image//gl3.png");
        p1Frames[LEFT][3]  = iLoadImage("Image//gl4.png");

        p1Frames[RIGHT][0] = iLoadImage("Image//gr1.png");
        p1Frames[RIGHT][1] = iLoadImage("Image//gr2.png");
        p1Frames[RIGHT][2] = iLoadImage("Image//gr3.png");
        p1Frames[RIGHT][3] = iLoadImage("Image//gr4.png");

        // Player 2: Arrow keys -> Up/Down/Left/Right.
        p2Frames[UP][0]    = iLoadImage("Image//rb1.png");
        p2Frames[UP][1]    = iLoadImage("Image//rb2.png");
        p2Frames[UP][2]    = iLoadImage("Image//rb3.png");
        p2Frames[UP][3]    = iLoadImage("Image//rb4.png");

        p2Frames[DOWN][0]  = iLoadImage("Image//rf1.png");
        p2Frames[DOWN][1]  = iLoadImage("Image//rf2.png");
        p2Frames[DOWN][2]  = iLoadImage("Image//rf3.png");
        p2Frames[DOWN][3]  = iLoadImage("Image//rf4.png");

        p2Frames[LEFT][0]  = iLoadImage("Image//rl1.png");
        p2Frames[LEFT][1]  = iLoadImage("Image//rl2.png");
        p2Frames[LEFT][2]  = iLoadImage("Image//rl3.png");
        p2Frames[LEFT][3]  = iLoadImage("Image//rl4.png");

        p2Frames[RIGHT][0] = iLoadImage("Image//rr1.png");
        p2Frames[RIGHT][1] = iLoadImage("Image//rr2.png");
        p2Frames[RIGHT][2] = iLoadImage("Image//rr3.png");
        p2Frames[RIGHT][3] = iLoadImage("Image//rr4.png");

        imagesLoaded = true;
    }

    static void reset()
    {
        p1Frame = 0;
        p2Frame = 0;
        p1Counter = 0;
        p2Counter = 0;
        p1Direction = DOWN;
        p2Direction = DOWN;
    }

    static void updatePlayer1(GameObject* player)
    {
        if (player == 0) return;

        bool moving = false;

        // Keep the same directional priority as the existing movement code.
        if (isKeyPressed('d'))
        {
            p1Direction = RIGHT;
            moving = true;
        }
        if (isKeyPressed('w'))
        {
            p1Direction = UP;
            moving = true;
        }
        if (isKeyPressed('a'))
        {
            p1Direction = LEFT;
            moving = true;
        }
        if (isKeyPressed('s'))
        {
            p1Direction = DOWN;
            moving = true;
        }

        if (!moving)
        {
            p1Counter = 0;
            p1Frame = 0;
            player->imgIndex = p1Frames[p1Direction][0];
            return;
        }

        p1Counter++;
        if (p1Counter >= FRAME_DELAY)
        {
            p1Counter = 0;
            p1Frame = (p1Frame + 1) % 4;
        }

        player->imgIndex = p1Frames[p1Direction][p1Frame];
    }

    static void updatePlayer2(GameObject* player)
    {
        if (player == 0) return;

        bool moving = false;

        // Keep the same movement input system already used by iMain.cpp.
        if (isSpecialKeyPressed(GLUT_KEY_RIGHT))
        {
            p2Direction = RIGHT;
            moving = true;
        }
        if (isSpecialKeyPressed(GLUT_KEY_UP))
        {
            p2Direction = UP;
            moving = true;
        }
        if (isSpecialKeyPressed(GLUT_KEY_LEFT))
        {
            p2Direction = LEFT;
            moving = true;
        }
        if (isSpecialKeyPressed(GLUT_KEY_DOWN))
        {
            p2Direction = DOWN;
            moving = true;
        }

        if (!moving)
        {
            p2Counter = 0;
            p2Frame = 0;
            player->imgIndex = p2Frames[p2Direction][0];
            return;
        }

        p2Counter++;
        if (p2Counter >= FRAME_DELAY)
        {
            p2Counter = 0;
            p2Frame = (p2Frame + 1) % 4;
        }

        player->imgIndex = p2Frames[p2Direction][p2Frame];
    }
}

#endif
