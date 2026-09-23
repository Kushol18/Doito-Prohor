#ifndef GAME_END_HPP
#define GAME_END_HPP

#include "iGraphics.h"
#include "MainMenu.hpp"
#include "PlayerData.hpp"
#include "Leaderboard.hpp"
#include "Stopwatch.hpp"
#include "Audios.hpp"
#include "GameObject.hpp"
#include <cmath>

extern int gameState;
extern bool game_completion;
extern int completionTimeSeconds;
extern GameObject* player1;
extern GameObject* player2;
extern int left;
extern int right;
extern bool isRuinCompleted[7];

namespace GameEnd
{
    static const int GAME_WIN_STATE = 8;
    static const int GAME_OVER_STATE = 9;
    static const int MESSAGE_STATE = 10;
    static const int CREDITS_SCROLL_STATE = 11;

    static const int HITBOX_COLLECT_X1 = 800;
    static const int HITBOX_COLLECT_Y1 = 90;
    static const int HITBOX_COLLECT_X2 = 1200;
    static const int HITBOX_COLLECT_Y2 = 300;

    static int messageImg = -1;
    static int winImg = -1;
    static int gameOverImg = -1;
    static int creditImg = -1;
    static bool imagesLoaded = false;

    static bool messageActive = false;
    static bool winActive = false;
    static bool gameOverActive = false;
    static bool pendingCredits = false;
    static bool creditsScrollActive = false;
    static bool portalPending = false;
    static bool portalPlayed = false;
    static ULONGLONG portalStartTick = 0;
    static ULONGLONG winStartTick = 0;
    static double creditScrollY = 0.0;
    static const double CREDIT_SCROLL_SPEED = 0.08;

    inline bool isInsideCollect(int mx, int my)
    {
        return mx >= HITBOX_COLLECT_X1 && mx <= HITBOX_COLLECT_X2 &&
               my >= HITBOX_COLLECT_Y1 && my <= HITBOX_COLLECT_Y2;
    }

    inline void initialize()
    {
        if (imagesLoaded) return;

        char messagePath[] = "Image//message.png";
        char winPath[] = "Image//gamewin.png";
        char gameOverPath[] = "Image//gameover.png";
        char creditPath[] = "Image//credit.png";

        messageImg = (int)iLoadImage(messagePath);
        winImg = (int)iLoadImage(winPath);
        gameOverImg = (int)iLoadImage(gameOverPath);
        creditImg = (int)iLoadImage(creditPath);
        imagesLoaded = true;
    }

    inline void resetSession()
    {
        messageActive = false;
        winActive = false;
        gameOverActive = false;
        pendingCredits = false;
        creditsScrollActive = false;
        portalPending = false;
        portalPlayed = false;
        portalStartTick = 0;
        winStartTick = 0;
        creditScrollY = 0.0;
    }

    inline bool areAllStructuresCompleted()
    {
        for (int variant = 1; variant <= 6; ++variant)
            if (!isRuinCompleted[variant]) return false;
        return true;
    }

    inline bool nearFinalPortal(GameObject* player, bool playerOne)
    {
        if (!player) return false;
        int expectedMap = playerOne ? 4 : 13;
        if (player->mapID != expectedMap) return false;

        double portalX = playerOne ? 500.0 : 1300.0;
        double portalY = 555.0;
        double px = player->x + player->width / 2.0;
        double py = player->y + player->height / 2.0;
        double dx = px - portalX;
        double dy = py - portalY;
        return (dx * dx + dy * dy) <= (105.0 * 105.0);
    }

    inline void showRewardMessage()
    {
        if (messageActive || winActive || gameOverActive) return;
        initialize();
        messageActive = true;
        Stopwatch::pause();
        gameState = MESSAGE_STATE;
    }

    inline void handleMessageClick(int& currentState, int mx, int my)
    {
        if (currentState != MESSAGE_STATE) return;
        if (!isInsideCollect(mx, my)) return;

        messageActive = false;
        currentState = DoitoProhorMenu::PLAY_STATE;
        Stopwatch::resume();
    }

    inline void triggerGameOver()
    {
        if (gameOverActive || winActive) return;

        initialize();
        gameOverActive = true;
        messageActive = false;
        portalPending = false;
        pendingCredits = false;
        Stopwatch::stopPermanent();
        completionTimeSeconds = -1;
        game_completion = false;
        Audios::stopGameplayBackground();
        Audios::stopTerminalAudio();
        Audios::playGameOver();
        gameState = GAME_OVER_STATE;
    }

    inline void triggerWin()
    {
        if (winActive || gameOverActive) return;

        initialize();
        portalPending = false;
        winActive = true;
        game_completion = true;
        Stopwatch::stopPermanent();
        completionTimeSeconds = Stopwatch::getElapsedSeconds();
        PlayerData::completionTimeSeconds = completionTimeSeconds;
        pendingCredits = true;
        winStartTick = GetTickCount64();

        Audios::stopGameplayBackground();
        Audios::stopTerminalAudio();
        Audios::playWin();
        gameState = GAME_WIN_STATE;
    }

    inline void checkFinalPortalActivation()
    {
        if (winActive || gameOverActive || portalPending) return;
        if (!areAllStructuresCompleted()) return;
        if (!nearFinalPortal(player1, true) || !nearFinalPortal(player2, false)) return;
        if (!isKeyPressed('e') || !isKeyPressed('0')) return;

        portalPending = true;
        portalPlayed = true;
        portalStartTick = GetTickCount64();
        Audios::playPortal();
    }

    inline void updateTerminalSequence()
    {
        if (portalPending)
        {
            if (GetTickCount64() - portalStartTick >= 3056ULL)
                triggerWin();
            return;
        }

        if (pendingCredits && gameState == GAME_WIN_STATE)
        {
            // The uploaded win.mp3 is about two seconds long. The MCI status
            // check is the source of truth; the small elapsed guard avoids a
            // startup race immediately after opening the file.
            ULONGLONG elapsed = GetTickCount64() - winStartTick;
            if (elapsed >= 500ULL && !Audios::isPlaying(Audios::WIN_ALIAS))
            {
                pendingCredits = false;
                creditsScrollActive = true;
                creditScrollY = 0.0;
                Audios::playCredits();
                gameState = CREDITS_SCROLL_STATE;
            }
        }

        if (creditsScrollActive && gameState == CREDITS_SCROLL_STATE)
        {
            const double scaledCreditHeight = 1097.0;
            const double maxScroll = scaledCreditHeight - 980.0;
            creditScrollY += CREDIT_SCROLL_SPEED;
            if (creditScrollY >= maxScroll)
            {
                creditScrollY = maxScroll;
                creditsScrollActive = false;
                gameState = DoitoProhorMenu::CREDITS_STATE;
            }
        }
    }

    inline void checkGameOver()
    {
        if (winActive || gameOverActive) return;
        if ((player1 && player1->hp <= 0) || (player2 && player2->hp <= 0))
            triggerGameOver();
    }

    inline void drawMessage(int screenWidth, int screenHeight)
    {
        if (!imagesLoaded) initialize();
        if (messageImg >= 0)
            iShowImage(0, 0, screenWidth, screenHeight, (unsigned int)messageImg);
    }

    inline void drawWin(int screenWidth, int screenHeight)
    {
        if (!imagesLoaded) initialize();
        if (winImg >= 0)
            iShowImage(0, 0, screenWidth, screenHeight, (unsigned int)winImg);
    }

    inline void drawGameOver(int screenWidth, int screenHeight)
    {
        if (!imagesLoaded) initialize();
        if (gameOverImg >= 0)
            iShowImage(0, 0, screenWidth, screenHeight, (unsigned int)gameOverImg);
    }

    inline void drawCreditsScroll(int screenWidth, int screenHeight)
    {
        if (!imagesLoaded) initialize();
        if (creditImg >= 0)
            iShowImage(0, -(int)creditScrollY, screenWidth, 1097, (unsigned int)creditImg);
    }

    inline bool handleGameOverClick(int& currentState, int mx, int my)
    {
        if (currentState != GAME_OVER_STATE) return false;

        // The uploaded game-over image places Main Menu at the lower center.
        if (mx >= 650 && mx <= 1270 && my >= 30 && my <= 190)
        {
            currentState = DoitoProhorMenu::MAIN_MENU_STATE;
            resetSession();
            Audios::stopGameplayBackground();
            Audios::stopTerminalAudio();
            Audios::playMenu();
            return true;
        }
        return false;
    }
}

#endif
