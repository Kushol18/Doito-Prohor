#ifndef MINI_MAP_HPP
#define MINI_MAP_HPP

#include "iGraphics.h"
#include "GameObject.hpp"

namespace MiniMap
{
    // Each mini-map is exactly 100x100 pixels, made from four 50x50 tiles.
    static const int MAP_SIZE = 100;
    static const int TILE_SIZE = 50;

    // These positions keep the mini-maps on the left/right sides of Title.png.
    static const int PLAYER1_X = 350;
    static const int PLAYER2_X = 1470;
    static const int MAP_Y = 820;

    // Main overworld dimensions used by the actual game maps.
    static const double WORLD_WIDTH = 920.0;
    static const double WORLD_HEIGHT = 650.0;

    // Player 2 map objects use an x offset of 960 in the main viewport.
    static const double PLAYER2_WORLD_OFFSET_X = 960.0;

    // Image handles for the mini-map backgrounds.
    static int player1Bg = -1;

    // Player 2 positions:
    // 1x2 = top-left
    // 1x1 = top-right
    // 2x1 = bottom-left
    // 2x2 = bottom-right
    static int player2Bg11 = -1;
    static int player2Bg12 = -1;
    static int player2Bg21 = -1;
    static int player2Bg22 = -1;

    static bool loaded = false;

    static void initialize()
    {
        if (loaded) return;

        // iLoadImage in this iGraphics version expects a writable char array.
        // These paths intentionally use separate writable arrays.
        char player1Path[]  = "Image//bg1world1.png";

        // Player 2 mini-map background correction:
        // 1x2 -> bg4world2.png
        // 1x1 -> bg12world2.png
        // 2x1 -> bg12world2.png
        // 2x2 -> bg3world2.png
        char player2_11Path[] = "Image//bg12world2.png";
        char player2_12Path[] = "Image//bg4world2.png";
        char player2_21Path[] = "Image//bg12world2.png";
        char player2_22Path[] = "Image//bg3world2.png";

        player1Bg   = iLoadImage(player1Path);
        player2Bg11 = iLoadImage(player2_11Path);
        player2Bg12 = iLoadImage(player2_12Path);
        player2Bg21 = iLoadImage(player2_21Path);
        player2Bg22 = iLoadImage(player2_22Path);

        loaded = true;
    }

    static void drawTile(int x, int y, int image)
    {
        if (image != -1)
        {
            iShowImage(x, y, TILE_SIZE, TILE_SIZE, image);
        }
    }

    static void drawYellowBorder(int x, int y)
    {
        // Thick yellow border, drawn inside the selected 50x50 tile.
        iSetColor(255, 255, 0);
        iFilledRectangle(x, y, TILE_SIZE, 3);
        iFilledRectangle(x, y + TILE_SIZE - 3, TILE_SIZE, 3);
        iFilledRectangle(x, y, 3, TILE_SIZE);
        iFilledRectangle(x + TILE_SIZE - 3, y, 3, TILE_SIZE);
    }

    // Player 1 quadrant positions:
    // 1x2 = top-left, 1x1 = top-right,
    // 2x1 = bottom-left, 2x2 = bottom-right.
    static int player1SelectedTile(int mapID)
    {
        if (mapID == 4) return 0; // 1x2: top-left
        if (mapID == 2) return 1; // 1x1: top-right
        if (mapID == 3) return 2; // 2x1: bottom-left
        if (mapID == 1) return 3; // 2x2: bottom-right
        return -1;
    }

    // Player 2 quadrant positions:
    // 1x2 = top-left, 1x1 = top-right,
    // 2x1 = bottom-left, 2x2 = bottom-right.
    static int player2SelectedTile(int mapID)
    {
        if (mapID == 11) return 0; // 1x2: top-left
        if (mapID == 13) return 1; // 1x1: top-right
        if (mapID == 10) return 2; // 2x1: bottom-left
        if (mapID == 12) return 3; // 2x2: bottom-right
        return -1;
    }

    // Return the mini-map origin of an overworld quadrant.
    static bool getTileOriginForMap(int mapID, int baseX, int baseY, int* tileX, int* tileY)
    {
        if (!tileX || !tileY) return false;

        switch (mapID)
        {
        // Player 1 / World 1
        case 4: *tileX = baseX;             *tileY = baseY + TILE_SIZE; return true; // top-left
        case 2: *tileX = baseX + TILE_SIZE; *tileY = baseY + TILE_SIZE; return true; // top-right
        case 3: *tileX = baseX;             *tileY = baseY;             return true; // bottom-left
        case 1: *tileX = baseX + TILE_SIZE; *tileY = baseY;             return true; // bottom-right

        // Player 2 / World 2
        case 11: *tileX = baseX;             *tileY = baseY + TILE_SIZE; return true; // top-left
        case 13: *tileX = baseX + TILE_SIZE; *tileY = baseY + TILE_SIZE; return true; // top-right
        case 10: *tileX = baseX;             *tileY = baseY;             return true; // bottom-left
        case 12: *tileX = baseX + TILE_SIZE; *tileY = baseY;             return true; // bottom-right
        default: return false;
        }
    }

    // Convert a main-world coordinate to a point in a particular 50x50 tile.
    static bool worldToMiniMap(
        int mapID,
        double worldX,
        double worldY,
        int baseX,
        int baseY,
        double* miniX,
        double* miniY
    )
    {
        if (!miniX || !miniY) return false;

        int tileX = 0;
        int tileY = 0;
        if (!getTileOriginForMap(mapID, baseX, baseY, &tileX, &tileY))
            return false;

        double localX = worldX;

        // Player 2 world maps are stored in the same coordinate system as the
        // second viewport, so remove the 960-pixel world offset first.
        if (mapID >= 10 && mapID <= 13)
            localX -= PLAYER2_WORLD_OFFSET_X;

        // Keep coordinates inside the 920x650 map area.
        if (localX < 0.0) localX = 0.0;
        if (localX > WORLD_WIDTH) localX = WORLD_WIDTH;

        double localY = worldY;
        if (localY < 0.0) localY = 0.0;
        if (localY > WORLD_HEIGHT) localY = WORLD_HEIGHT;

        *miniX = tileX + (localX / WORLD_WIDTH) * TILE_SIZE;
        *miniY = tileY + (localY / WORLD_HEIGHT) * TILE_SIZE;
        return true;
    }

    // Only show objects that are actually visible in the corresponding
    // overworld gameplay map.
    static bool shouldDrawObject(const GameObject& obj)
    {
        if (obj.isHidden)
            return false;

        if (obj.id == OBJ_COLLECTIBLE && obj.isCollected)
            return false;

        // The main iDraw() skips inactive effects.
        if (obj.id == OBJ_EFFECT && !obj.isActivated)
            return false;

        return true;
    }

    // Draw the same GameObject sprites that exist in the main world, but
    // reduced to the mini-map's scale.
    static void drawObjectMarkers(int baseX, int baseY, bool player2World)
    {
        GameObject* allObjects = getAllObjects();
        const int totalObjects = getObjectCount();

        for (int i = 0; i < totalObjects; ++i)
        {
            GameObject* obj = &allObjects[i];
            if (!obj)
                continue;

            // Each mini-map only represents its own four overworld maps.
            if (player2World)
            {
                if (obj->mapID < 10 || obj->mapID > 13)
                    continue;
            }
            else
            {
                if (obj->mapID < 1 || obj->mapID > 4)
                    continue;
            }

            if (!shouldDrawObject(*obj))
                continue;

            if (obj->imgIndex < 0)
                continue;

            double miniX = 0.0;
            double miniY = 0.0;

            if (!worldToMiniMap(
                    obj->mapID,
                    obj->x,
                    obj->y,
                    baseX,
                    baseY,
                    &miniX,
                    &miniY))
            {
                continue;
            }

            // Scale the object's main-world dimensions into the mini-map.
            double miniW = (obj->width / WORLD_WIDTH) * TILE_SIZE;
            double miniH = (obj->height / WORLD_HEIGHT) * TILE_SIZE;

            // Very small objects (collectibles, switches, players) would
            // otherwise become nearly invisible at 100x100, so keep a small
            // minimum visible size.
            if (miniW < 3.0) miniW = 3.0;
            if (miniH < 3.0) miniH = 3.0;

            // Keep the miniature inside the selected 50x50 tile.
            int tileX = 0;
            int tileY = 0;
            if (!getTileOriginForMap(obj->mapID, baseX, baseY, &tileX, &tileY))
                continue;

            if (miniX + miniW > tileX + TILE_SIZE)
                miniW = (tileX + TILE_SIZE) - miniX;
            if (miniY + miniH > tileY + TILE_SIZE)
                miniH = (tileY + TILE_SIZE) - miniY;

            if (miniW <= 0.0 || miniH <= 0.0)
                continue;

            iShowImage(
                (int)miniX,
                (int)miniY,
                (int)miniW,
                (int)miniH,
                obj->imgIndex
            );
        }
    }

    static void drawPlayer1(int mapID)
    {
        const int x = PLAYER1_X;
        const int y = MAP_Y;

        // Same bg1world1.png used in all four positions.
        drawTile(x,             y + TILE_SIZE, player1Bg); // 1x2: top-left
        drawTile(x + TILE_SIZE, y + TILE_SIZE, player1Bg); // 1x1: top-right
        drawTile(x,             y,               player1Bg); // 2x1: bottom-left
        drawTile(x + TILE_SIZE, y,               player1Bg); // 2x2: bottom-right

        // Show all visible main-world objects on the four P1 quadrants.
        drawObjectMarkers(x, y, false);

        const int selected = player1SelectedTile(mapID);

        if (selected == 0) drawYellowBorder(x, y + TILE_SIZE);
        else if (selected == 1) drawYellowBorder(x + TILE_SIZE, y + TILE_SIZE);
        else if (selected == 2) drawYellowBorder(x, y);
        else if (selected == 3) drawYellowBorder(x + TILE_SIZE, y);
    }

    static void drawPlayer2(int mapID)
    {
        const int x = PLAYER2_X;
        const int y = MAP_Y;

        // Corrected Player 2 backgrounds:
        // 1x2: bg4world2.png
        // 1x1: bg12world2.png
        // 2x1: bg12world2.png
        // 2x2: bg3world2.png
        drawTile(x,             y + TILE_SIZE, player2Bg11); // 1x2: top-left
        drawTile(x + TILE_SIZE, y + TILE_SIZE, player2Bg12); // 1x1: top-right
        drawTile(x,             y,               player2Bg21); // 2x1: bottom-left
        drawTile(x + TILE_SIZE, y,               player2Bg22); // 2x2: bottom-right

        // Show all visible main-world objects on the four P2 quadrants.
        drawObjectMarkers(x, y, true);

        const int selected = player2SelectedTile(mapID);

        if (selected == 0) drawYellowBorder(x, y + TILE_SIZE);
        else if (selected == 1) drawYellowBorder(x + TILE_SIZE, y + TILE_SIZE);
        else if (selected == 2) drawYellowBorder(x, y);
        else if (selected == 3) drawYellowBorder(x + TILE_SIZE, y);
    }

    static void draw(int player1MapID, int player2MapID)
    {
        initialize();

        // Keep both mini-maps at exactly 100x100 pixels.
        drawPlayer1(player1MapID);
        drawPlayer2(player2MapID);
    }
}

#endif
