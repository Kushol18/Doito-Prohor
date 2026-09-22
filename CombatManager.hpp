#ifndef COMBAT_MANAGER_HPP
#define COMBAT_MANAGER_HPP

#include "PlayerCombat.hpp"
#include "Enemy.hpp"

// Function call: Player deals damage to Enemy
inline void damageEnemy(Enemy* enemy, int amount) {
    if (enemy) {
        enemy->takeDamage(amount);
    }
}

// Function call: Player attacks Enemy within melee range
inline void processPlayerAttack(GameObject* player, Enemy* enemy, int damage, double reach = 12.0) {
    if (!player || !enemy) return;

    if (checkCollision(player, enemy, reach)) {
        damageEnemy(enemy, damage);
    }
}

// Updates player status and triggers enemy attack calls
inline void updateCombat(GameObject* player, Enemy* enemy, double deltaTime, int activeMapID) {
    if (!player) return;

    // Update player i-frames
    updatePlayerCombat(player, deltaTime);

    // Update enemy AI & trigger damage call when attack hits
    if (enemy && enemy->isAlive) {
        bool attackLanded = enemy->update(player, deltaTime, activeMapID);
        if (attackLanded) {
            damagePlayer(player, enemy->attackDamage);
        }
    }
}


#endif