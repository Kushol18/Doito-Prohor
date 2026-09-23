#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "GameObject.hpp"
#include "CollisionManager.hpp"
#include "PlayerCombat.hpp"
#include "Audios.hpp"
#include "iGraphics.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>

class Enemy : public GameObject
{
public:
    static const double ATTACK_DISTANCE;
    static const double ATTACK_COOLDOWN;
    static const double PLAYER_INVULNERABILITY_TIME;

    int attackDamage;
    double moveSpeed;
    double detectionRange;
    double attackRange;
    double attackCooldown;
    double timeSinceLastAttack;
    bool isAlive;
    double blockChance;

    enum Direction { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3 };
    int frames[4][3];
    int attackLeft[4];
    int attackRight[4];
    int frame;
    int frameCounter;
    int attackFrame;
    int attackCounter;
    bool attacking;
    Direction facing;
    Direction lastMoveDirection;

    Enemy()
    {
        id = OBJ_OBSTACLES;
        x = 0.0;
        y = 0.0;
        spawnX = 0.0;
        spawnY = 0.0;
        imgIndex = -1;
        width = 38;
        height = 58;
        collisionWidth = 30;
        collisionHeight = 44;
        hp = 100;
        maxHp = 100;
        invulnerableTimer = 0.0;
        isDead = false;
        isHidden = false;
        attackDamage = 20;
        moveSpeed = 1.7;
        detectionRange = 320.0;
        attackRange = ATTACK_DISTANCE;
        attackCooldown = ATTACK_COOLDOWN;
        timeSinceLastAttack = ATTACK_COOLDOWN;
        isAlive = true;
        blockChance = 0.25;
        frame = 0;
        frameCounter = 0;
        attackFrame = 0;
        attackCounter = 0;
        attacking = false;
        facing = DOWN;
        lastMoveDirection = DOWN;

        int r, c;
        for (r = 0; r < 4; ++r)
            for (c = 0; c < 3; ++c)
                frames[r][c] = -1;
        for (c = 0; c < 4; ++c)
        {
            attackLeft[c] = -1;
            attackRight[c] = -1;
        }
    }

    void init(double startX, double startY, int map, int damage, double blockPercent)
    {
        x = startX;
        y = startY;
        spawnX = startX;
        spawnY = startY;
        mapID = map;
        hp = 100;
        maxHp = 100;
        attackDamage = damage;
        attackRange = ATTACK_DISTANCE;
        attackCooldown = ATTACK_COOLDOWN;
        timeSinceLastAttack = ATTACK_COOLDOWN;
        isAlive = true;
        isHidden = false;
        isDead = false;
        invulnerableTimer = 0.0;
        blockChance = blockPercent;
        frame = 0;
        frameCounter = 0;
        attackFrame = 0;
        attackCounter = 0;
        attacking = false;
        facing = DOWN;
        lastMoveDirection = DOWN;
    }

    void resetCombatState()
    {
        x = spawnX;
        y = spawnY;
        hp = maxHp = 100;
        invulnerableTimer = 0.0;
        isDead = false;
        isAlive = true;
        isHidden = false;
        timeSinceLastAttack = attackCooldown;
        frame = 0;
        frameCounter = 0;
        attackFrame = 0;
        attackCounter = 0;
        attacking = false;
        facing = DOWN;
        lastMoveDirection = DOWN;
    }

    bool tryTakeDamage(int amount, double effectX, double effectY)
    {
        if (!isAlive || isDead || invulnerableTimer > 0.0)
            return false;

        double roll = (double)rand() / (double)RAND_MAX;
        if (roll < blockChance)
        {
            Audios::playBlock();
            registerHitEffect(effectX, effectY, mapID);
            invulnerableTimer = 0.15;
            return false;
        }

        hp -= amount;
        if (hp < 0) hp = 0;
        invulnerableTimer = 0.25;
        registerHitEffect(effectX, effectY, mapID);
        Audios::playHit();

        if (hp <= 0)
        {
            hp = 0;
            isDead = true;
            isAlive = false;
            isHidden = true;
        }
        return true;
    }

    void takeDamage(int amount)
    {
        tryTakeDamage(amount, x, y + height * 0.5);
    }

    // Distance between the nearest edges of the enemy/player collision boxes.
    // This keeps the requested attack range exactly 48 pixels and avoids
    // sprite-center distance being affected by different sprite sizes.
    double distanceToPlayer(GameObject* targetPlayer) const
    {
        if (!targetPlayer) return 1000000000.0;

        double selfW = (collisionWidth > 0.0) ? collisionWidth : width;
        double selfH = (collisionHeight > 0.0) ? collisionHeight : height;
        double targetW = (targetPlayer->collisionWidth > 0.0) ? targetPlayer->collisionWidth : targetPlayer->width;
        double targetH = (targetPlayer->collisionHeight > 0.0) ? targetPlayer->collisionHeight : targetPlayer->height;

        double selfLeft = x + (width - selfW) * 0.5;
        double selfRight = selfLeft + selfW;
        double selfBottom = y;
        double selfTop = selfBottom + selfH;

        double targetLeft = targetPlayer->x + (targetPlayer->width - targetW) * 0.5;
        double targetRight = targetLeft + targetW;
        double targetBottom = targetPlayer->y;
        double targetTop = targetBottom + targetH;

        double gapX = 0.0;
        if (selfRight < targetLeft) gapX = targetLeft - selfRight;
        else if (targetRight < selfLeft) gapX = selfLeft - targetRight;

        double gapY = 0.0;
        if (selfTop < targetBottom) gapY = targetBottom - selfTop;
        else if (targetTop < selfBottom) gapY = selfBottom - targetTop;

        return std::sqrt(gapX * gapX + gapY * gapY);
    }

    bool update(GameObject* targetPlayer, double deltaTime, int activeMapID)
    {
        if (!isAlive || isHidden || !targetPlayer || targetPlayer->mapID != mapID || mapID != activeMapID)
            return false;

        if (invulnerableTimer > 0.0)
        {
            invulnerableTimer -= deltaTime;
            if (invulnerableTimer < 0.0) invulnerableTimer = 0.0;
        }

        timeSinceLastAttack += deltaTime;

        double enemyCenterX = x + width * 0.5;
        double enemyCenterY = y + height * 0.5;
        double playerCenterX = targetPlayer->x + targetPlayer->width * 0.5;
        double playerCenterY = targetPlayer->y + targetPlayer->height * 0.5;
        double dx = playerCenterX - enemyCenterX;
        double dy = playerCenterY - enemyCenterY;
        double centerDistance = std::sqrt(dx * dx + dy * dy);
        double attackDistance = distanceToPlayer(targetPlayer);

        if (std::fabs(dx) >= std::fabs(dy))
            facing = (dx >= 0.0) ? RIGHT : LEFT;
        else
            facing = (dy >= 0.0) ? UP : DOWN;

        bool moving = false;

        // EXACTLY 48 pixels of attack reach.
        if (attackDistance <= ATTACK_DISTANCE)
        {
            if (timeSinceLastAttack >= attackCooldown)
            {
                timeSinceLastAttack = 0.0;
                attacking = true;
                attackFrame = 0;
                attackCounter = 0;

                // Return an attack event. EnemySystem applies the actual
                // player damage after this function returns.
                return true;
            }
        }
        else if (centerDistance <= detectionRange)
        {
            moving = moveTowardsPlayer(dx, dy, centerDistance, activeMapID);
        }

        if (attacking)
        {
            ++attackCounter;
            if (attackCounter >= 5)
            {
                attackCounter = 0;
                ++attackFrame;
                if (attackFrame >= 4)
                {
                    attackFrame = 0;
                    attacking = false;
                }
            }
        }

        if (moving)
        {
            ++frameCounter;
            if (frameCounter >= 7)
            {
                frameCounter = 0;
                frame = (frame + 1) % 3;
            }
            lastMoveDirection = facing;
        }
        else
        {
            frameCounter = 0;
            frame = 0;
        }

        return false;
    }

    void draw(int activeMapID)
    {
        if (!isAlive || isHidden || mapID != activeMapID) return;

        int texture = frames[facing][frame];
        if (attacking)
        {
            if (facing == LEFT) texture = attackLeft[attackFrame];
            else if (facing == RIGHT) texture = attackRight[attackFrame];
            else texture = (lastMoveDirection == LEFT) ? attackLeft[attackFrame] : attackRight[attackFrame];
        }

        if (texture >= 0)
            iShowImage((int)x, (int)y, (int)width, (int)height, (unsigned int)texture);

        double ratio = (maxHp > 0) ? (double)hp / (double)maxHp : 0.0;
        if (ratio < 0.0) ratio = 0.0;
        if (ratio > 1.0) ratio = 1.0;
        iSetColor(35, 35, 35);
        iFilledRectangle((int)x, (int)y + (int)height + 4, (int)width, 5);
        iSetColor(220, 40, 40);
        iFilledRectangle((int)x, (int)y + (int)height + 4, (int)(width * ratio), 5);
    }

    static void registerHitEffect(double effectX, double effectY, int activeMapID)
    {
        hitX = effectX;
        hitY = effectY;
        hitMapID = activeMapID;
        hitTimer = 0.22;
    }

    static void updateHitEffect(double deltaTime)
    {
        if (hitTimer > 0.0)
        {
            hitTimer -= deltaTime;
            if (hitTimer < 0.0) hitTimer = 0.0;
        }
    }

    static void drawHitEffect(int activeMapID)
    {
        if (hitTimer <= 0.0 || hitMapID != activeMapID || hitImage < 0) return;
        iShowImage((int)hitX - 8, (int)hitY + 8, 42, 56, (unsigned int)hitImage);
    }

    static int loadImage(const char* path)
    {
        char buffer[128];
        std::strcpy(buffer, path);
        return (int)iLoadImage(buffer);
    }

    static double hitX;
    static double hitY;
    static int hitMapID;
    static double hitTimer;
    static int hitImage;

private:
    double spawnX;
    double spawnY;

    bool moveTowardsPlayer(double dx, double dy, double distance, int activeMapID)
    {
        if (distance <= 0.0) return false;

        double stepX = (dx / distance) * moveSpeed;
        double stepY = (dy / distance) * moveSpeed;
        double nextX = x + stepX;
        double nextY = y + stepY;

        if (!checkCollisionForMap(this, nextX, nextY, activeMapID))
        {
            x = nextX;
            y = nextY;
            return true;
        }
        if (!checkCollisionForMap(this, nextX, y, activeMapID))
        {
            x = nextX;
            return true;
        }
        if (!checkCollisionForMap(this, x, nextY, activeMapID))
        {
            y = nextY;
            return true;
        }
        return false;
    }
};

const double Enemy::ATTACK_DISTANCE = 48.0;
const double Enemy::ATTACK_COOLDOWN = 1.0;
const double Enemy::PLAYER_INVULNERABILITY_TIME = 0.8;
double Enemy::hitX = 0.0;
double Enemy::hitY = 0.0;
int Enemy::hitMapID = -1;
double Enemy::hitTimer = 0.0;
int Enemy::hitImage = -1;

namespace EnemySystem
{
    static const int ENEMY_COUNT = 4;
    static Enemy enemies[ENEMY_COUNT];
    static bool initialized = false;
    static bool assetsLoaded = false;
    static int enemyTextures[4][3];
    static int attackLeftTextures[4];
    static int attackRightTextures[4];

    void loadAssets()
    {
        if (assetsLoaded) return;

        enemyTextures[Enemy::UP][0] = Enemy::loadImage("Image//eb1.png");
        enemyTextures[Enemy::UP][1] = Enemy::loadImage("Image//eb2.png");
        enemyTextures[Enemy::UP][2] = Enemy::loadImage("Image//eb3.png");
        enemyTextures[Enemy::DOWN][0] = Enemy::loadImage("Image//ef1.png");
        enemyTextures[Enemy::DOWN][1] = Enemy::loadImage("Image//ef2.png");
        enemyTextures[Enemy::DOWN][2] = Enemy::loadImage("Image//ef3.png");
        enemyTextures[Enemy::LEFT][0] = Enemy::loadImage("Image//el1.png");
        enemyTextures[Enemy::LEFT][1] = Enemy::loadImage("Image//el2.png");
        enemyTextures[Enemy::LEFT][2] = Enemy::loadImage("Image//el3.png");
        enemyTextures[Enemy::RIGHT][0] = Enemy::loadImage("Image//er1.png");
        enemyTextures[Enemy::RIGHT][1] = Enemy::loadImage("Image//er2.png");
        enemyTextures[Enemy::RIGHT][2] = Enemy::loadImage("Image//er3.png");

        int i;
        for (i = 0; i < 4; ++i)
        {
            char path[64];
            std::sprintf(path, "Image//ela%d.png", i + 1);
            attackLeftTextures[i] = Enemy::loadImage(path);
            std::sprintf(path, "Image//era%d.png", i + 1);
            attackRightTextures[i] = Enemy::loadImage(path);
        }

        Enemy::hitImage = Enemy::loadImage("Image//hit.png");
        assetsLoaded = true;
    }

    void assignAssets(Enemy* enemy)
    {
        int r, c;
        for (r = 0; r < 4; ++r)
            for (c = 0; c < 3; ++c)
                enemy->frames[r][c] = enemyTextures[r][c];
        for (c = 0; c < 4; ++c)
        {
            enemy->attackLeft[c] = attackLeftTextures[c];
            enemy->attackRight[c] = attackRightTextures[c];
        }
    }

    bool validSpawnPosition(Enemy* enemy, double startX, double startY, int mapID)
    {
        enemy->mapID = mapID;
        return !checkCollisionForMap(enemy, startX, startY, mapID);
    }

    void chooseSpawn(Enemy* enemy, int mapID, double xOff)
    {
        int attempt;
        for (attempt = 0; attempt < 500; ++attempt)
        {
            double startX = xOff + 90.0 + (double)(rand() % 650);
            double startY = 80.0 + (double)(rand() % 460);
            if (validSpawnPosition(enemy, startX, startY, mapID))
            {
                enemy->init(startX, startY, mapID,
                            (mapID == 102 || mapID == 105) ? 20 : 10,
                            (mapID == 103 || mapID == 106) ? 0.45 : 0.25);
                assignAssets(enemy);
                return;
            }
        }

        enemy->init(xOff + 200.0, 120.0, mapID,
                    (mapID == 102 || mapID == 105) ? 20 : 10,
                    (mapID == 103 || mapID == 106) ? 0.45 : 0.25);
        assignAssets(enemy);
    }

    void initialize()
    {
        loadAssets();
        if (initialized) return;

        chooseSpawn(&enemies[0], 102, 40.0);
        chooseSpawn(&enemies[1], 103, 40.0);
        chooseSpawn(&enemies[2], 105, 1000.0);
        chooseSpawn(&enemies[3], 106, 1000.0);
        initialized = true;
    }

    void resetSession()
    {
        if (!initialized) initialize();
        chooseSpawn(&enemies[0], 102, 40.0);
        chooseSpawn(&enemies[1], 103, 40.0);
        chooseSpawn(&enemies[2], 105, 1000.0);
        chooseSpawn(&enemies[3], 106, 1000.0);
        Enemy::hitTimer = 0.0;
    }

    Enemy* enemyForMap(int mapID)
    {
        int i;
        for (i = 0; i < ENEMY_COUNT; ++i)
            if (enemies[i].mapID == mapID) return &enemies[i];
        return 0;
    }

    void playerAttack(GameObject* player)
    {
        if (!player || !player->hasSword) return;

        Enemy* enemy = enemyForMap(player->mapID);
        if (!enemy || !enemy->isAlive) return;

        if (checkCollision(player, enemy, 28.0))
        {
            int damage = (player->mapID == 102 || player->mapID == 105) ? 20 : 10;
            enemy->tryTakeDamage(damage, enemy->x, enemy->y + enemy->height * 0.5);
        }
    }

    // Centralized damage application. The player invulnerability timer is
    // explicitly counted down here, so a successful first hit cannot make
    // subsequent enemy hits permanently ineffective.
    bool applyEnemyDamage(GameObject* player, Enemy* enemy)
    {
        if (!player || !enemy || !enemy->isAlive) return false;

        if (player->invulnerableTimer > 0.0)
            return false;

        int previousHP = player->hp;
        damagePlayer(player, enemy->attackDamage);

        // Guarantee the cooldown exists even when the project's older
        // PlayerCombat implementation does not set it itself.
        player->invulnerableTimer = Enemy::PLAYER_INVULNERABILITY_TIME;

        if (player->hp < previousHP)
        {
            Enemy::registerHitEffect(player->x, player->y, player->mapID);
            Audios::playHit();
            return true;
        }
        return false;
    }

    void update(GameObject* player1, GameObject* player2, double deltaTime, int left, int right)
    {
        Enemy* e1 = enemyForMap(left);
        Enemy* e2 = enemyForMap(right);

        // Explicitly maintain player damage cooldowns here. This does not
        // replace PlayerCombat; it makes enemy damage independent of it.
        if (player1 && player1->invulnerableTimer > 0.0)
        {
            player1->invulnerableTimer -= deltaTime;
            if (player1->invulnerableTimer < 0.0) player1->invulnerableTimer = 0.0;
        }
        if (player2 && player2->invulnerableTimer > 0.0)
        {
            player2->invulnerableTimer -= deltaTime;
            if (player2->invulnerableTimer < 0.0) player2->invulnerableTimer = 0.0;
        }

        if (e1 && player1 && e1->update(player1, deltaTime, left))
            applyEnemyDamage(player1, e1);

        if (e2 && player2 && e2->update(player2, deltaTime, right))
            applyEnemyDamage(player2, e2);

        Enemy::updateHitEffect(deltaTime);
    }

    void draw(int left, int right)
    {
        Enemy* e1 = enemyForMap(left);
        Enemy* e2 = enemyForMap(right);
        if (e1) e1->draw(left);
        if (e2) e2->draw(right);
        Enemy::drawHitEffect(left);
        Enemy::drawHitEffect(right);
    }
}

#endif
