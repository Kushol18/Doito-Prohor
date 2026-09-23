#ifndef AUDIOS_HPP
#define AUDIOS_HPP

#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <cstring>
#include <cstdio>

#pragma comment(lib, "winmm.lib")

namespace Audios
{
    static const char MENU_ALIAS[] = "dp_menu";
    static const char GAMEPLAY_ALIAS[] = "dp_gameplay";
    static const char HIT_ALIAS[] = "dp_hit";
    static const char BLOCK_ALIAS[] = "dp_block";
    static const char COLLECT_ALIAS[] = "dp_collect";
    static const char ENTRANCE_ALIAS[] = "dp_entrance";
    static const char PORTAL_ALIAS[] = "dp_portal";
    static const char WIN_ALIAS[] = "dp_win";
    static const char GAMEOVER_ALIAS[] = "dp_gameover";
    static const char CREDITS_ALIAS[] = "dp_credits";

    static const char MENU_FILE[] = "Audio//Menu.mp3";
    static const char GAMEPLAY_FILE[] = "Audio//gameplayBG.mp3";
    static const char HIT_FILE[] = "Audio//hit.mp3";
    static const char BLOCK_FILE[] = "Audio//block.mp3";
    static const char COLLECT_FILE[] = "Audio//collectable.mp3";
    static const char ENTRANCE_FILE[] = "Audio//Entrance.mp3";
    static const char PORTAL_FILE[] = "Audio//portal.mp3";
    static const char WIN_FILE[] = "Audio//win.mp3";
    static const char GAMEOVER_FILE[] = "Audio//gameover.mp3";
    static const char CREDITS_FILE[] = "Audio//credits.mp3";

    inline void closeAlias(const char* alias)
    {
        char command[128];
        std::sprintf(command, "close %s", alias);
        mciSendStringA(command, 0, 0, 0);
    }

    inline void openAlias(const char* alias, const char* file)
    {
        closeAlias(alias);

        char command[512];
        std::sprintf(command, "open \"%s\" type mpegvideo alias %s", file, alias);
        mciSendStringA(command, 0, 0, 0);
    }

    inline void playAlias(const char* alias, bool repeat = false)
    {
        char command[160];
        if (repeat)
        {
            std::sprintf(command, "play %s repeat", alias);
        }
        else
        {
            std::sprintf(command, "play %s from 0", alias);
        }
        mciSendStringA(command, 0, 0, 0);
    }

    inline bool isPlaying(const char* alias)
    {
        char result[64] = {0};
        char command[128];
        std::sprintf(command, "status %s mode", alias);
        mciSendStringA(command, result, sizeof(result), 0);
        return std::strcmp(result, "playing") == 0;
    }

    inline void initialize()
    {
        // The audio files are loaded lazily through their exact discovered
        // filenames, so no duplicate mixer/player system is required.
    }

    inline void playMenu()
    {
        openAlias(MENU_ALIAS, MENU_FILE);
        playAlias(MENU_ALIAS, true);
    }

    inline void stopMenu()
    {
        closeAlias(MENU_ALIAS);
    }

    inline void playGameplayBackground()
    {
        stopMenu();
        openAlias(GAMEPLAY_ALIAS, GAMEPLAY_FILE);
        playAlias(GAMEPLAY_ALIAS, true);
    }

    inline void stopGameplayBackground()
    {
        closeAlias(GAMEPLAY_ALIAS);
    }

    inline void playHit()
    {
        openAlias(HIT_ALIAS, HIT_FILE);
        playAlias(HIT_ALIAS, false);
    }

    inline void playBlock()
    {
        openAlias(BLOCK_ALIAS, BLOCK_FILE);
        playAlias(BLOCK_ALIAS, false);
    }

    inline void playCollectible()
    {
        openAlias(COLLECT_ALIAS, COLLECT_FILE);
        playAlias(COLLECT_ALIAS, false);
    }

    inline void playEntrance()
    {
        openAlias(ENTRANCE_ALIAS, ENTRANCE_FILE);
        playAlias(ENTRANCE_ALIAS, false);
    }

    inline void playPortal()
    {
        openAlias(PORTAL_ALIAS, PORTAL_FILE);
        playAlias(PORTAL_ALIAS, false);
    }

    inline void playWin()
    {
        openAlias(WIN_ALIAS, WIN_FILE);
        playAlias(WIN_ALIAS, false);
    }

    inline void playGameOver()
    {
        openAlias(GAMEOVER_ALIAS, GAMEOVER_FILE);
        playAlias(GAMEOVER_ALIAS, false);
    }

    inline void playCredits()
    {
        openAlias(CREDITS_ALIAS, CREDITS_FILE);
        playAlias(CREDITS_ALIAS, false);
    }

    inline void stopTerminalAudio()
    {
        closeAlias(WIN_ALIAS);
        closeAlias(GAMEOVER_ALIAS);
        closeAlias(CREDITS_ALIAS);
    }

    inline void stopAll()
    {
        closeAlias(MENU_ALIAS);
        closeAlias(GAMEPLAY_ALIAS);
        closeAlias(HIT_ALIAS);
        closeAlias(BLOCK_ALIAS);
        closeAlias(COLLECT_ALIAS);
        closeAlias(ENTRANCE_ALIAS);
        closeAlias(PORTAL_ALIAS);
        closeAlias(WIN_ALIAS);
        closeAlias(GAMEOVER_ALIAS);
        closeAlias(CREDITS_ALIAS);
    }
}

#endif
