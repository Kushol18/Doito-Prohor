#ifndef PLAYER_HEALTH_HPP
#define PLAYER_HEALTH_HPP

#include "iGraphics.h"
#include "GameObject.hpp"
#include <cstdio>

namespace PlayerHealth
{
    static int healthImages[11] = {0};
    static bool imagesLoaded = false;
    static bool prevP1BandageKey = false;
    static bool prevP2BandageKey = false;

    static const int BANDAGE_ITEM = 3;
    static const int MAX_HEALTH = 100;

    inline void initialize()
    {
        if (imagesLoaded) return;
        for (int i = 1; i <= 10; ++i)
        {
            char path[64];
            std::sprintf(path, "Image//health%d.png", i);
            healthImages[i] = (int)iLoadImage(path);
        }
        imagesLoaded = true;
    }

    inline void reset()
    {
        prevP1BandageKey = false;
        prevP2BandageKey = false;
    }

    inline void initPlayer(GameObject* player)
    {
        if (!player) return;
        player->hp = MAX_HEALTH;
        player->maxHp = MAX_HEALTH;
        player->invulnerableTimer = 0.0;
        player->isDead = false;
    }

    inline void useBandage(GameObject* player)
    {
        if (!player) return;
        if (globalInventory[BANDAGE_ITEM] <= 0) return;
        if (player->hp >= MAX_HEALTH) return;

        player->hp = MAX_HEALTH;
        globalInventory[BANDAGE_ITEM]--;
        if (globalInventory[BANDAGE_ITEM] < 0) globalInventory[BANDAGE_ITEM] = 0;
    }

    inline void updateConsumableInput(GameObject* player1, GameObject* player2)
    {
        bool p1Down = isKeyPressed('r') != 0;
        bool p2Down = isKeyPressed('6') != 0;

        if (p1Down && !prevP1BandageKey) useBandage(player1);
        if (p2Down && !prevP2BandageKey) useBandage(player2);

        prevP1BandageKey = p1Down;
        prevP2BandageKey = p2Down;
    }

    inline int imageForHealth(int hp)
    {
        int slot = (hp + 9) / 10;
        if (slot < 1) slot = 1;
        if (slot > 10) slot = 10;
        return slot;
    }

    inline void drawBar(GameObject* player, int x, int y, const char* label)
    {
        if (!player) return;

        int slot = imageForHealth(player->hp);
        if (healthImages[slot] != 0)
        {
            iShowImage(x, y, 250, 83, healthImages[slot]);
        }

        iSetColor(255, 255, 255);
        char labelBuffer[64];
        std::sprintf(labelBuffer, "%s: %d/100", label, player->hp);
        iText(x + 8, y + 5, labelBuffer, GLUT_BITMAP_HELVETICA_18);
    }

    inline void draw(GameObject* player1, GameObject* player2, int screen1X, int screen2X, int baseY)
    {
        drawBar(player1, screen1X + 5, baseY, "Player 1 Health");
        drawBar(player2, screen2X + 5, baseY, "Player 2 Health");
    }
}

#endif
