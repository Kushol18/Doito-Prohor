#ifndef PLAYER_COMBAT_HPP
#define PLAYER_COMBAT_HPP

#include "GameObject.hpp"
#include "iGraphics.h"

// Initialize HP for any GameObject player pointer
inline void initPlayerCombat(GameObject* player, int maxHealth = 100) {
    if (!player) return;
    player->hp = maxHealth;
    player->maxHp = maxHealth;
    player->invulnerableTimer = 0.0;
    player->isDead = false;
}

// Function call: Deals damage to a GameObject player (handles i-frames)
inline void damagePlayer(GameObject* player, int amount) {
    if (!player || player->isDead || player->invulnerableTimer > 0.0) return;

    player->hp -= amount;
    if (player->hp <= 0) {
        player->hp = 0;
        player->isDead = true;
    }
    player->invulnerableTimer = 0.8; // 0.8 seconds of invulnerability
}

// Update invulnerability timer in main loop
inline void updatePlayerCombat(GameObject* player, double deltaTime) {
    if (!player) return;
    if (player->invulnerableTimer > 0.0) {
        player->invulnerableTimer -= deltaTime;
        if (player->invulnerableTimer < 0.0) {
            player->invulnerableTimer = 0.0;
        }
    }
}

// Draws player health bar HUD
inline void drawPlayerHealthBar(GameObject* player, int screenX, int screenY, int width, int height) {
    if (!player) return; // Removed player->isDead check so HUD stays visible on death

    iSetColor(50, 50, 50);
    iFilledRectangle(screenX, screenY, width, height);

    if (player->invulnerableTimer > 0.0) {
        iSetColor(255, 255, 255);
    } else {
        iSetColor(40, 200, 60);
    }

    int safeMaxHp = (player->maxHp > 0) ? player->maxHp : 1;
    double ratio = (double)player->hp / (double)safeMaxHp;
    if (ratio < 0.0) ratio = 0.0;

    iFilledRectangle(screenX + 2, screenY + 2, (int)((width - 4) * ratio), height - 4);
}

#endif
