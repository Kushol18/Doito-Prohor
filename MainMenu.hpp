#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include <cstdlib>

namespace DoitoProhorMenu
{
    // Reuse the existing gameState values from iMain.cpp:
    // 0 = Main Menu, 1 = Existing Gameplay, 2 = Story,
    // 3 = Leaderboard, 4 = Options, 5 = Credits.
    static const int MAIN_MENU_STATE = 0;
    static const int PLAY_STATE = 1;
    static const int STORY_STATE = 2;
    static const int LEADERBOARD_STATE = 3;
    static const int OPTIONS_STATE = 4;
    static const int CREDITS_STATE = 5;

    struct MenuButton
    {
        int x1;
        int y1;
        int x2;
        int y2;
    };

    // Menu images are loaded once from main().
    static int mainMenuImg = -1;
    static int storyImg = -1;
    static int creditImg = -1;
    static int backOnlyImg = -1;
    static bool imagesLoaded = false;

    // These hitboxes are in the 1920x980 game window coordinate system.
    // The main_menu.png image is scaled from 1756x896 to fill that window.
    static const MenuButton playButton        = { 720, 555, 1140, 610 };
    static const MenuButton storyButton       = { 720, 465, 1140, 520 };
    static const MenuButton leaderboardButton = { 720, 380, 1140, 445 };
    static const MenuButton optionsButton     = { 720, 300, 1140, 360 };
    static const MenuButton creditsButton     = { 720, 215, 1140, 270 };
    static const MenuButton exitButton         = { 720, 130, 1140, 185 };

    // Shared Back hitbox for story.png, back_only.png, and credit.png.
    static const MenuButton backButton = { 760, 25, 1160, 170 };

    static bool isInsideButton(const MenuButton& button, int mx, int my)
    {
        return mx >= button.x1 && mx <= button.x2 &&
               my >= button.y1 && my <= button.y2;
    }

    static bool isMenuState(int state)
    {
        return state == MAIN_MENU_STATE ||
               state == STORY_STATE ||
               state == LEADERBOARD_STATE ||
               state == OPTIONS_STATE ||
               state == CREDITS_STATE;
    }

    static void initialize()
    {
        if (imagesLoaded) return;

        mainMenuImg = iLoadImage("Image//main_menu.png");
        storyImg = iLoadImage("Image//story.png");
        creditImg = iLoadImage("Image//credit.png");
        backOnlyImg = iLoadImage("Image//back_only.png");

        imagesLoaded = true;
    }

    static void draw(int gameState, int screenWidth, int screenHeight)
    {
        if (gameState == MAIN_MENU_STATE)
        {
            iShowImage(0, 0, screenWidth, screenHeight, mainMenuImg);
        }
        else if (gameState == STORY_STATE)
        {
            iShowImage(0, 0, screenWidth, screenHeight, storyImg);
        }
        else if (gameState == LEADERBOARD_STATE)
        {
            iShowImage(0, 0, screenWidth, screenHeight, backOnlyImg);
        }
        else if (gameState == OPTIONS_STATE)
        {
            iShowImage(0, 0, screenWidth, screenHeight, backOnlyImg);
        }
        else if (gameState == CREDITS_STATE)
        {
            iShowImage(0, 0, screenWidth, screenHeight, creditImg);
        }
    }

    static void handleClick(int& gameState, int mx, int my)
    {
        if (gameState == MAIN_MENU_STATE)
        {
            if (isInsideButton(playButton, mx, my))
            {
                gameState = PLAY_STATE;
            }
            else if (isInsideButton(storyButton, mx, my))
            {
                gameState = STORY_STATE;
            }
            else if (isInsideButton(leaderboardButton, mx, my))
            {
                gameState = LEADERBOARD_STATE;
            }
            else if (isInsideButton(optionsButton, mx, my))
            {
                gameState = OPTIONS_STATE;
            }
            else if (isInsideButton(creditsButton, mx, my))
            {
                gameState = CREDITS_STATE;
            }
            else if (isInsideButton(exitButton, mx, my))
            {
                exit(0);
            }
        }
        else if (gameState == STORY_STATE ||
                 gameState == LEADERBOARD_STATE ||
                 gameState == OPTIONS_STATE ||
                 gameState == CREDITS_STATE)
        {
            if (isInsideButton(backButton, mx, my))
            {
                gameState = MAIN_MENU_STATE;
            }
        }
    }
}

#endif
