#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "GameObject.hpp"
#include "CollisionManager.hpp"
#include "iGraphics.h"
#include <cmath>
#include <cstring>

class Enemy : public GameObject {
public:
    int attackDamage;
    double moveSpeed;
    double detectionRange;
    double attackRange;
    double attackCooldown;
    double timeSinceLastAttack;
    bool isAlive;

    char spriteLeft[100];
    char spriteRight[100];
    char currentSprite[100];
    bool facingRight;

    Enemy() {
        hp = 50;
        maxHp = 50;
        attackDamage = 10;
        moveSpeed = 2.0;
        detectionRange = 250.0;
        attackRange = 35.0;
        attackCooldown = 1.0;
        timeSinceLastAttack = 1.0;
        isAlive = true;
        facingRight = true;
    }

    void init(double startX, double startY, int map, int health, int damage, double speed,
              double detectRadius, double atkRadius, const char* imgLeft, const char* imgRight) {
        x = startX;
        y = startY;
        mapID = map;
        hp = health;
        maxHp = health;
        attackDamage = damage;
        moveSpeed = speed;
        detectionRange = detectRadius;
        attackRange = atkRadius;
        isAlive = true;
        isHidden = false;
        width = 32;
        height = 32;

        strcpy_s(spriteLeft, imgLeft);
        strcpy_s(spriteRight, imgRight);
        strcpy_s(currentSprite, imgRight);
    }

    // Function call: Enemy takes damage
    void takeDamage(int amount) {
		// Uses inherited isDead and invulnerableTimer for protection
		if (isDead || invulnerableTimer > 0.0) return;

		hp -= amount;
		if (hp <= 0) {
			hp = 0;
			isDead = true;
			isHidden = true;
		}
		invulnerableTimer = 0.4; // 0.4s cooldown between hits
	}

    // AI logic update: returns true when an attack connects
    bool update(GameObject* targetPlayer, double deltaTime, int activeMapID) {
        if (!isAlive || isHidden || !targetPlayer || targetPlayer->mapID != mapID) return false;

        timeSinceLastAttack += deltaTime;

        double enemyCenterX = x + (width / 2.0);
        double enemyCenterY = y + (height / 2.0);
        double playerCenterX = targetPlayer->x + (targetPlayer->width / 2.0);
        double playerCenterY = targetPlayer->y + (targetPlayer->height / 2.0);

        double dx = playerCenterX - enemyCenterX;
        double dy = playerCenterY - enemyCenterY;
        double distance = std::sqrt(dx * dx + dy * dy);

        // Turn left/right image
        if (dx >= 0) {
            facingRight = true;
            strcpy_s(currentSprite, spriteRight);
        } else {
            facingRight = false;
            strcpy_s(currentSprite, spriteLeft);
        }

        if (distance <= detectionRange) {
            if (distance <= attackRange) {
                if (timeSinceLastAttack >= attackCooldown) {
                    timeSinceLastAttack = 0.0;
                    return true; // Signal hit
                }
            } else {
                moveTowardsPlayer(dx, dy, distance, activeMapID);
            }
        }
        return false;
    }

    void draw(int activeMapID) {
        if (!isAlive || mapID != activeMapID) return;

        // Render enemy sprite image
        iShowBMP2((int)x, (int)y, currentSprite, 0xFFFFFF);

        // Render overhead health bar
        double ratio = (double)hp / (double)maxHp;
        iSetColor(40, 40, 40);
        iFilledRectangle((int)x, (int)y + (int)height + 4, (int)width, 4);
        iSetColor(220, 40, 40);
        iFilledRectangle((int)x, (int)y + (int)height + 4, (int)(width * ratio), 4);
    }

private:
    void moveTowardsPlayer(double dx, double dy, double distance, int activeMapID) {
        if (distance <= 0) return;

        double dirX = (dx / distance) * moveSpeed;
        double dirY = (dy / distance) * moveSpeed;

        double nextX = x + dirX;
        double nextY = y + dirY;

        if (!checkCollisionForMap(this, nextX, nextY, activeMapID)) {
            x = nextX;
            y = nextY;
            return;
        }

        if (!checkCollisionForMap(this, nextX, y, activeMapID)) {
            x = nextX;
            return;
        }

        if (!checkCollisionForMap(this, x, nextY, activeMapID)) {
            y = nextY;
            return;
        }
    }
};

#endif