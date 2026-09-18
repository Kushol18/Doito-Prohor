#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include <cstdlib>
#include "PlayerData.hpp"
#include "Leaderboard.hpp"

namespace DoitoProhorMenu
{
    // Reuse the existing gameState values from iMain.cpp:
    // 0 = Main Menu, 1 = Existing Gameplay, 2 = Story,
    // 3 = Leaderboard, 4 = Options, 5 = Credits, 6 = Pause,
    // 7 = Player Data.
    static const int MAIN_MENU_STATE = 0;
    static const int PLAY_STATE = 1;
    static const int STORY_STATE = 2;
    static const int LEADERBOARD_STATE = 3;
    static const int OPTIONS_STATE = 4;
    static const int CREDITS_STATE = 5;
	static const int PLAYER_DATA_STATE = 7;

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
    static const MenuButton playButton        = { 720, 535, 1140, 590 };
    static const MenuButton storyButton       = { 720, 445, 1140, 500 };
    static const MenuButton leaderboardButton = { 720, 360, 1140, 425 };
    static const MenuButton optionsButton     = { 720, 280, 1140, 340 };
    static const MenuButton creditsButton     = { 720, 195, 1140, 250 };
    static const MenuButton exitButton        = { 720, 110, 1140, 165 };

    // Shared Back hitbox for story.png, back_only.png, and credit.png.
	// and the new player-data/leaderboard screens.
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
               state == CREDITS_STATE ||
               state == PLAYER_DATA_STATE;
    }

    static void initialize()
    {
        if (!imagesLoaded)
        {
            char mainMenuPath[] = "Image//main_menu.png";
            char storyPath[] = "Image//story.png";
            char creditPath[] = "Image//credit.png";
            char backPath[] = "Image//back_only.png";

            mainMenuImg = iLoadImage(mainMenuPath);
            storyImg = iLoadImage(storyPath);
            creditImg = iLoadImage(creditPath);
            backOnlyImg = iLoadImage(backPath);
			imagesLoaded = true;
		}

		PlayerData::initialize();
        Leaderboard::initialize();
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
            Leaderboard::draw();
        }
        else if (gameState == OPTIONS_STATE)
        {
            iShowImage(0, 0, screenWidth, screenHeight, backOnlyImg);
        }
        else if (gameState == CREDITS_STATE)
        {
            iShowImage(0, 0, screenWidth, screenHeight, creditImg);
        }
		else if (gameState == PLAYER_DATA_STATE)
        {
            PlayerData::draw();
        }
    }

    static void handleClick(int& gameState, int mx, int my)
    {
        if (gameState == MAIN_MENU_STATE)
        {
            if (isInsideButton(playButton, mx, my))
            {
                PlayerData::beginNewEntry();
                gameState = PLAYER_DATA_STATE;
            }
            else if (isInsideButton(storyButton, mx, my))
            {
                gameState = STORY_STATE;
            }
            else if (isInsideButton(leaderboardButton, mx, my))
            {
                Leaderboard::refresh();
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
		else if (gameState == PLAYER_DATA_STATE)
        {
            int action = PlayerData::handleClick(mx, my);
            if (action == 1)
            {
                gameState = MAIN_MENU_STATE;
            }
            else if (action == 2)
            {
                gameState = PLAY_STATE;
            }
        }
        else if (gameState == LEADERBOARD_STATE)
        {
            if (Leaderboard::handleClick(mx, my))
            {
                gameState = MAIN_MENU_STATE;
            }
        }
        else if (gameState == STORY_STATE ||
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
