#ifndef MAZE_MANAGER_HPP
#define MAZE_MANAGER_HPP

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <vector>
#include <cstdlib>
#include <ctime>
#include <utility>
#include "GameObject.hpp"
#include "CollisionManager.hpp"
#include "iGraphics.h"

static int wallImg = -1;

const int MAZE_COLS = 15;
const int MAZE_ROWS = 11;

static const int MAZE_BANDAGE_ITEM = 3;
static const int MAZE_SWORD_ITEM = 4;

inline void spawnWallTile(double x, double y, double tileSize, int mapID) {
    GameObject* wall = createObject(OBJ_WALL, x, y, wallImg, (int)tileSize, (int)tileSize, (int)tileSize, (int)tileSize, -1);
    if (wall != 0) wall->mapID = mapID;
}

inline void generateDFSGrid(int grid[MAZE_ROWS][MAZE_COLS]) {
    for (int r = 0; r < MAZE_ROWS; r++)
        for (int c = 0; c < MAZE_COLS; c++) grid[r][c] = 1;

    int cellRows = MAZE_ROWS / 2;
    int cellCols = MAZE_COLS / 2;
    std::vector<std::vector<bool> > visited(cellRows, std::vector<bool>(cellCols, false));
    std::vector<std::pair<int, int> > stack;

    visited[0][0] = true;
    grid[1][1] = 0;
    stack.push_back(std::make_pair(0, 0));

    int dR[] = {1, -1, 0, 0};
    int dC[] = {0, 0, -1, 1};

    while (!stack.empty()) {
        std::pair<int, int> current = stack.back();
        int cr = current.first;
        int cc = current.second;

        std::vector<int> neighbors;
        for (int i = 0; i < 4; i++) {
            int nr = cr + dR[i];
            int nc = cc + dC[i];
            if (nr >= 0 && nr < cellRows && nc >= 0 && nc < cellCols && !visited[nr][nc])
                neighbors.push_back(i);
        }

        if (!neighbors.empty()) {
            int dir = neighbors[rand() % neighbors.size()];
            int nr = cr + dR[dir];
            int nc = cc + dC[dir];

            int wallTileR = (cr * 2 + 1) + dR[dir];
            int wallTileC = (cc * 2 + 1) + dC[dir];
            int targetTileR = nr * 2 + 1;
            int targetTileC = nc * 2 + 1;

            grid[wallTileR][wallTileC] = 0;
            grid[targetTileR][targetTileC] = 0;
            visited[nr][nc] = true;
            stack.push_back(std::make_pair(nr, nc));
        } else {
            stack.pop_back();
        }
    }

    grid[1][0] = 0;
    grid[MAZE_ROWS - 2][MAZE_COLS - 1] = 0;
}

inline bool mazeCandidateValid(GameObject* item, double x, double y, int mapID,
                               double xOff, double yOff, double viewportW, double viewportH)
{
    double tileW = viewportW / MAZE_COLS;
    double tileH = viewportH / MAZE_ROWS;
    double minX = xOff + tileW * 1.1;
    double maxX = xOff + viewportW - tileW * 1.8 - item->width;
    double minY = yOff + tileH * 1.1;
    double maxY = yOff + viewportH - tileH * 1.8 - item->height;

    if (x < minX || x > maxX || y < minY || y > maxY) return false;
    if (checkCollisionForMap(item, x, y, mapID)) return false;

    // Keep the two new maze collectibles separate from each other while still
    // using the existing collision system for walls/obstacles.
    GameObject* allObjects = getAllObjects();
    int count = getObjectCount();
    double candidateCenterX = x + item->width * 0.5;
    double candidateCenterY = y + item->height * 0.5;
    for (int i = 0; i < count; ++i)
    {
        GameObject* other = &allObjects[i];
        if (other == item || other->mapID != mapID || other->id != OBJ_COLLECTIBLE)
            continue;
        if (other->itemType != MAZE_BANDAGE_ITEM && other->itemType != MAZE_SWORD_ITEM)
            continue;

        double otherCenterX = other->x + other->width * 0.5;
        double otherCenterY = other->y + other->height * 0.5;
        double dx = candidateCenterX - otherCenterX;
        double dy = candidateCenterY - otherCenterY;
        if ((dx * dx + dy * dy) < (42.0 * 42.0)) return false;
    }

    return true;
}

inline void spawnMazeCollectible(int mapID, double xOff, double yOff, double viewportW,
                                 double viewportH, int imageIndex, int itemType,
                                 const int grid[MAZE_ROWS][MAZE_COLS])
{
    GameObject* item = createObject(OBJ_COLLECTIBLE, xOff, yOff, imageIndex, 28, 28, 24, 24, -1);
    if (!item) return;
    item->mapID = mapID;
    item->itemType = itemType;

    double lastValidX = item->x;
    double lastValidY = item->y;
    bool foundValidPosition = mazeCandidateValid(item, lastValidX, lastValidY, mapID, xOff, yOff, viewportW, viewportH);

    for (int attempt = 0; attempt < 500; ++attempt)
    {
        double tileW = viewportW / MAZE_COLS;
        double tileH = viewportH / MAZE_ROWS;
        double x = xOff + tileW + (double)(rand() % (int)(viewportW - 3 * tileW));
        double y = yOff + tileH + (double)(rand() % (int)(viewportH - 3 * tileH));

        if (mazeCandidateValid(item, x, y, mapID, xOff, yOff, viewportW, viewportH))
        {
            lastValidX = x;
            lastValidY = y;
            foundValidPosition = true;
            break;
        }
    }

    // Search deterministically through the maze area if random attempts failed.
    if (!foundValidPosition)
    {
        double tileW = viewportW / MAZE_COLS;
        double tileH = viewportH / MAZE_ROWS;
        for (int r = 1; r < MAZE_ROWS - 1 && !foundValidPosition; ++r)
        {
            for (int c = 1; c < MAZE_COLS - 1; ++c)
            {
                double x = xOff + c * tileW + (tileW - item->width) * 0.5;
                double y = yOff + r * tileH + (tileH - item->height) * 0.5;
                if (grid && grid[r][c] == 0 &&
                    mazeCandidateValid(item, x, y, mapID, xOff, yOff, viewportW, viewportH))
                {
                    lastValidX = x;
                    lastValidY = y;
                    foundValidPosition = true;
                    break;
                }
            }
        }
    }

    item->x = lastValidX;
    item->y = lastValidY;
}

inline void buildDFSMazeForPlayer(int mapID, double xOff, double yOff, double viewportW,
                                  double viewportH, int bandageImg, int swordImg) {
    int grid[MAZE_ROWS][MAZE_COLS];
    generateDFSGrid(grid);

    double tileW = viewportW / MAZE_COLS;
    double tileH = viewportH / MAZE_ROWS;

    for (int r = 0; r < MAZE_ROWS; r++) {
        for (int c = 0; c < MAZE_COLS; c++) {
            if (grid[r][c] == 1) {
                double px = xOff + (c * tileW);
                double py = yOff + (r * tileH);
                GameObject* wall = createObject(OBJ_WALL, px, py, wallImg,
                                                (int)tileW, (int)tileH,
                                                tileW, tileH, -1);
                if (wall != 0) wall->mapID = mapID;
            }
        }
    }

    // Exactly one Bandage and one Sword are created for this maze/session.
    spawnMazeCollectible(mapID, xOff, yOff, viewportW, viewportH, bandageImg, MAZE_BANDAGE_ITEM, grid);
    spawnMazeCollectible(mapID, xOff, yOff, viewportW, viewportH, swordImg, MAZE_SWORD_ITEM, grid);
}

// In-place session randomizer: preserve exactly one existing Bandage and one existing Sword per maze.
inline void randomizeExistingMazeCollectibles()
{
    const double viewportW = 880.0;
    const double viewportH = 600.0;
    GameObject* allObjects = getAllObjects();
    int count = getObjectCount();

    for (int i = 0; i < count; ++i)
    {
        GameObject* item = &allObjects[i];
        if (item->id != OBJ_COLLECTIBLE) continue;
        if (item->itemType != MAZE_BANDAGE_ITEM && item->itemType != MAZE_SWORD_ITEM) continue;
        if (item->mapID < 101 || item->mapID > 106) continue;

        const double xOff = (item->mapID >= 104) ? 1000.0 : 40.0;
        const double oldX = item->x;
        const double oldY = item->y;
        double tileW = viewportW / MAZE_COLS;
        double tileH = viewportH / MAZE_ROWS;
        bool placed = false;

        for (int attempt = 0; attempt < 500; ++attempt)
        {
            double x = xOff + tileW + (double)(rand() % (int)(viewportW - 3 * tileW));
            double y = 40.0 + tileH + (double)(rand() % (int)(viewportH - 3 * tileH));
            if (mazeCandidateValid(item, x, y, item->mapID, xOff, 40.0, viewportW, viewportH))
            {
                item->x = x;
                item->y = y;
                placed = true;
                break;
            }
        }

        if (!placed)
        {
            item->x = oldX;
            item->y = oldY;
        }
    }
}

inline void buildAllMazes() {
    static bool seeded = false;
    if (seeded) return;

    srand((unsigned int)time(0));
    seeded = true;

    char bandagePath[] = "Image//bandage.png";
    char swordPath[] = "Image//sword.png";
    int bandageImg = (int)iLoadImage(bandagePath);
    int swordImg = (int)iLoadImage(swordPath);

    // P1 structures ST1-ST3 -> maze maps 101-103.
    buildDFSMazeForPlayer(101, 40.0, 40.0, 880.0, 600.0, bandageImg, swordImg);
    buildDFSMazeForPlayer(102, 40.0, 40.0, 880.0, 600.0, bandageImg, swordImg);
    buildDFSMazeForPlayer(103, 40.0, 40.0, 880.0, 600.0, bandageImg, swordImg);

    // P2 structures ST4-ST6 -> maze maps 104-106.
    buildDFSMazeForPlayer(104, 1000.0, 40.0, 880.0, 600.0, bandageImg, swordImg);
    buildDFSMazeForPlayer(105, 1000.0, 40.0, 880.0, 600.0, bandageImg, swordImg);
    buildDFSMazeForPlayer(106, 1000.0, 40.0, 880.0, 600.0, bandageImg, swordImg);
}

#endif
