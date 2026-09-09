#ifndef PAUSE_MENU_HPP
#define PAUSE_MENU_HPP

#include "MainMenu.hpp"
#include "iGraphics.h"

namespace PauseMenu
{
    // Existing gameState architecture: gameplay is 1, pause is 6.
    static const int PAUSED_STATE = 6;

    // The pause icon is displayed in the top-left corner of the 1920x980
    // gameplay screen.
    static const DoitoProhorMenu::MenuButton pauseButton = { 20, 890, 84, 954 };

    // Hitboxes are based on the visible Resume and Main Menu positions in
    // pause_menu.png after scaling it to the 1920x980 game window.
    static const DoitoProhorMenu::MenuButton resumeButton = { 670, 430, 1250, 555 };
    static const DoitoProhorMenu::MenuButton mainMenuButton = { 650, 305, 1270, 425 };

    static int pauseImg = -1;
    static int pauseMenuImg = -1;
    static bool imagesLoaded = false;

    static void initialize()
    {
        if (imagesLoaded) return;

        pauseImg = iLoadImage("Image//pause.png");
        pauseMenuImg = iLoadImage("Image//pause_menu.png");

        imagesLoaded = true;
    }

    static void drawPauseButton()
    {
        if (pauseImg != -1)
        {
            iShowImage(
                pauseButton.x1,
                pauseButton.y1,
                pauseButton.x2 - pauseButton.x1,
                pauseButton.y2 - pauseButton.y1,
                pauseImg
            );
        }
    }

    static void draw(int screenWidth, int screenHeight)
    {
        if (pauseMenuImg != -1)
        {
            iShowImage(0, 0, screenWidth, screenHeight, pauseMenuImg);
        }
    }

    // Handles the pause icon while gameplay is active.
    // It deliberately reuses MainMenu.hpp::isInsideButton(...).
    static bool handleGameplayClick(int& gameState, int mx, int my)
    {
        if (gameState != DoitoProhorMenu::PLAY_STATE)
        {
            return false;
        }

        if (DoitoProhorMenu::isInsideButton(pauseButton, mx, my))
        {
            gameState = PAUSED_STATE;
            return true;
        }

        return false;
    }

    // Handles only Resume/Main Menu while paused.
    // It deliberately reuses MainMenu.hpp::isInsideButton(...).
    static void handleClick(int& gameState, int mx, int my)
    {
        if (gameState != PAUSED_STATE)
        {
            return;
        }

        if (DoitoProhorMenu::isInsideButton(resumeButton, mx, my))
        {
            gameState = DoitoProhorMenu::PLAY_STATE;
        }
        else if (DoitoProhorMenu::isInsideButton(mainMenuButton, mx, my))
        {
            gameState = DoitoProhorMenu::MAIN_MENU_STATE;
        }
    }
}

#endif
