#ifndef MAZE_MANAGER_HPP
#define MAZE_MANAGER_HPP

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <vector>
#include <cstdlib>
#include <ctime>
#include <utility> // Required for std::make_pair
#include "GameObject.hpp"

static int wallImg = -1; 

const int MAZE_COLS = 15;
const int MAZE_ROWS = 11;

inline void spawnWallTile(double x, double y, double tileSize, int mapID) {
    GameObject* wall = createObject(OBJ_WALL, x, y, wallImg, tileSize, tileSize, tileSize, tileSize, -1);
    if (wall != 0) {
        wall->mapID = mapID;
    }
}

inline void generateDFSGrid(int grid[MAZE_ROWS][MAZE_COLS]) {
    for (int r = 0; r < MAZE_ROWS; r++) {
        for (int c = 0; c < MAZE_COLS; c++) {
            grid[r][c] = 1;
        }
    }

    int cellRows = MAZE_ROWS / 2;
    int cellCols = MAZE_COLS / 2;
    std::vector<std::vector<bool> > visited(cellRows, std::vector<bool>(cellCols, false));

    std::vector<std::pair<int, int> > stack;

    // Start at bottom-left cell (0,0) -> Maps to grid tile (1,1)
    visited[0][0] = true;
    grid[1][1] = 0;
    
    // FIX: Replaced {0, 0} with std::make_pair for C++03 compatibility
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
            if (nr >= 0 && nr < cellRows && nc >= 0 && nc < cellCols && !visited[nr][nc]) {
                neighbors.push_back(i);
            }
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
            
            // FIX: Replaced {nr, nc} with std::make_pair for C++03 compatibility
            stack.push_back(std::make_pair(nr, nc));
        } else {
            stack.pop_back();
        }
    }

    grid[1][0] = 0;                             // Start Entrance
    grid[MAZE_ROWS - 2][MAZE_COLS - 1] = 0;     // Exit Passage
}

inline void buildDFSMazeForPlayer(int mapID, double xOff, double yOff, double viewportW, double viewportH) {
    int grid[MAZE_ROWS][MAZE_COLS];
    generateDFSGrid(grid);

    double tileW = viewportW / MAZE_COLS;
    double tileH = viewportH / MAZE_ROWS;

    for (int r = 0; r < MAZE_ROWS; r++) {
        for (int c = 0; c < MAZE_COLS; c++) {
            if (grid[r][c] == 1) {
                double px = xOff + (c * tileW);
                double py = yOff + (r * tileH);
                
                GameObject* wall = createObject(OBJ_WALL, px, py, wallImg, tileW, tileH, tileW, tileH, -1);
                if (wall != 0) {
                    wall->mapID = mapID;
                }
            }
        }
    }
}

inline void buildAllMazes() {
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(0));
        seeded = true;
    }

    buildDFSMazeForPlayer(101, 40.0, 40.0, 880.0, 600.0);
    buildDFSMazeForPlayer(104, 1000.0, 40.0, 880.0, 600.0);
}

#endif