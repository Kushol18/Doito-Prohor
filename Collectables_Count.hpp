#define _CRT_SECURE_NO_WARNINGS
#ifndef COLLECTABLES_COUNT_HPP
#define COLLECTABLES_COUNT_HPP

#include "iGraphics.h"
#include "GameObject.hpp"
#include <cstdio>
#include <cstring>

namespace CollectablesCount
{
    static char BANDAGE_IMAGE[] = "Image//bandage.png";
    static char TIME_SHARD_IMAGE[] = "Image//timeshard.png";
    static char HOURGLASS_IMAGE[] = "Image//hourglass.png";
    static char FOOD_IMAGE[] = "Image//food.png";

    static const int ICON_SIZE = 44;
    static const int START_X = 1690;
    static const int SLOT_GAP = 56;
    static const int ICON_Y = 884;
    static const int COUNT_Y = 853;

    static int bandageImg = -1;
    static int timeShardImg = -1;
    static int hourglassImg = -1;
    static int foodImg = -1;
    static bool imagesLoaded = false;

    static void initialize()
    {
        if (imagesLoaded) return;

        bandageImg = iLoadImage(BANDAGE_IMAGE);
        timeShardImg = iLoadImage(TIME_SHARD_IMAGE);
        hourglassImg = iLoadImage(HOURGLASS_IMAGE);
        foodImg = iLoadImage(FOOD_IMAGE);
        imagesLoaded = true;
    }

    static void drawCenteredCount(int centerX, int count)
    {
        char countText[32];
        std::sprintf(countText, "x%d", count);
        int textWidth = (int)std::strlen(countText) * 7;
        iSetColor(255, 255, 255);
        iText(centerX - textWidth / 2, COUNT_Y, countText,
              GLUT_BITMAP_HELVETICA_18);
    }

    static void drawSlot(int x, int image, int count)
    {
        if (image != -1)
            iShowImage(x, ICON_Y, ICON_SIZE, ICON_SIZE, (unsigned int)image);
        drawCenteredCount(x + ICON_SIZE / 2, count);
    }

    static void draw()
    {
        if (!imagesLoaded) return;

        // Keep the existing top-right HUD compact while adding the new shared
        // Bandage and structure Hourglass counters.
        drawSlot(START_X + SLOT_GAP * 0, bandageImg, getSharedInventoryCount(3));
        drawSlot(START_X + SLOT_GAP * 1, timeShardImg, getSharedInventoryCount(0));
        drawSlot(START_X + SLOT_GAP * 2, hourglassImg, getSharedInventoryCount(2));
        drawSlot(START_X + SLOT_GAP * 3, foodImg, getSharedInventoryCount(1));
    }
}

#endif
