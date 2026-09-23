#ifndef COMBAT_MANAGER_HPP
#define COMBAT_MANAGER_HPP

#include "PlayerCombat.hpp"
#include "Enemy.hpp"

// Existing direct damage helper retained for compatibility.
inline void damageEnemy(Enemy* enemy, int amount) {
    if (enemy) enemy->takeDamage(amount);
}

// Existing API retained. It now uses the enemy's own combat/blocking rules.
inline void processPlayerAttack(GameObject* player, Enemy* enemy, int damage, double reach = 12.0) {
    if (!player || !enemy || !enemy->isAlive) return;
    if (checkCollision(player, enemy, reach)) {
        enemy->tryTakeDamage(damage, enemy->x, enemy->y + enemy->height * 0.5);
    }
}

// Existing per-enemy update API retained.
inline void updateCombat(GameObject* player, Enemy* enemy, double deltaTime, int activeMapID) {
    if (!player || !enemy || !enemy->isAlive) return;
    updatePlayerCombat(player, deltaTime);
    if (enemy->update(player, deltaTime, activeMapID)) {
        damagePlayer(player, enemy->attackDamage);
    }
}

// New integration bridge: iMain uses this rather than constructing another
// attack/pathfinding system. EnemySystem remains responsible for the actual AI.
namespace CombatManager {
    inline void handlePlayerSwordAttack(GameObject* player) {
        EnemySystem::playerAttack(player);
    }

    inline void updateAll(GameObject* player1, GameObject* player2,
                          int map1, int map2, double deltaTime) {
        EnemySystem::update(player1, player2, deltaTime, map1, map2);
    }

    inline void drawAll(int map1, int map2) {
        EnemySystem::draw(map1, map2);
    }
}

#endif
