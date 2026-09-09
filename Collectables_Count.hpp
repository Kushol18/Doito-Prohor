#ifndef COLLECTABLES_COUNT_HPP
#define COLLECTABLES_COUNT_HPP

#include "iGraphics.h"
#include "GameObject.hpp"
#include <cstdio>
#include <cstring>

namespace CollectablesCount
{
    // The project contains the existing assets under these actual filenames.
    static char TIME_SHARD_IMAGE[] = "Image//timeshard.png";
    static char FOOD_IMAGE[] = "Image//food.png";

    // Compact top-right HUD layout for the 1920x980 game window.
    static const int ICON_SIZE = 60;
    static const int TIME_SHARD_X = 1740;
    static const int FOOD_X = 1840;
    static const int ICON_Y = 875;
    static const int COUNT_Y = 850;

    static int timeShardImg = -1;
    static int foodImg = -1;
    static bool imagesLoaded = false;

    static void initialize()
    {
        if (imagesLoaded) return;

        timeShardImg = iLoadImage(TIME_SHARD_IMAGE);
        foodImg = iLoadImage(FOOD_IMAGE);

        imagesLoaded = true;
    }

    static void drawCenteredCount(int centerX, int count)
    {
        char countText[32];
        sprintf(countText, "x%d", count);

        // Approximate the bitmap font width so the count is centered
        // underneath its corresponding icon.
        int textWidth = (int)strlen(countText) * 7;
        iText(centerX - textWidth / 2, COUNT_Y, countText,
              GLUT_BITMAP_HELVETICA_18);
    }

    static void draw()
    {
        if (!imagesLoaded) return;

        // Time Shard is always rendered first and remains on the left.
        if (timeShardImg != -1)
        {
            iShowImage(TIME_SHARD_X, ICON_Y,
                       ICON_SIZE, ICON_SIZE, timeShardImg);
        }

        drawCenteredCount(
            TIME_SHARD_X + ICON_SIZE / 2,
            getSharedInventoryCount(0)
        );

        // Pet Food is always rendered second and remains on the right.
        if (foodImg != -1)
        {
            iShowImage(FOOD_X, ICON_Y,
                       ICON_SIZE, ICON_SIZE, foodImg);
        }

        drawCenteredCount(
            FOOD_X + ICON_SIZE / 2,
            getSharedInventoryCount(1)
        );
    }
}

#endif
