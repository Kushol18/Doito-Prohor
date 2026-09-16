#ifndef PLAYER_DATA_HPP
#define PLAYER_DATA_HPP

#include "iGraphics.h"
#include <string>
#include <cstring>

namespace PlayerData
{
    static const int STATE = 7;
    static const int MAX_NAME_LENGTH = 24;

    struct Button
    {
        int x1;
        int y1;
        int x2;
        int y2;
    };

    // Coordinates are for the 1920x980 game window after player_data.png
    // is scaled to fill the screen.
    static const Button teamField   = { 865, 548, 1295, 610 };
    static const Button player1Field = { 865, 445, 1295, 508 };
    static const Button player2Field = { 865, 344, 1295, 407 };
    static const Button backButton   = { 760, 25, 1160, 170 };
    static const Button startButton  = { 1220, 55, 1740, 155 };

    static int playerDataImg = -1;
    static int activeField = 0;
    static bool keysWereDown[512] = { false };
    static bool imagesLoaded = false;

    static std::string teamName = "";
    static std::string player1Name = "";
    static std::string player2Name = "";

    static bool isInside(const Button& button, int mx, int my)
    {
        return mx >= button.x1 && mx <= button.x2 &&
               my >= button.y1 && my <= button.y2;
    }

    static bool isNameCharacter(unsigned char key)
    {
        return (key >= 'A' && key <= 'Z') ||
               (key >= 'a' && key <= 'z') ||
               (key >= '0' && key <= '9') ||
               key == ' ';
    }

    static std::string& currentText()
    {
        if (activeField == 0) return teamName;
        if (activeField == 1) return player1Name;
        return player2Name;
    }

    static void drawText(int x, int y, const std::string& text)
    {
        char buffer[128];
        std::strncpy(buffer, text.c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
        iText(x, y, buffer, GLUT_BITMAP_HELVETICA_18);
    }

    static void initialize()
    {
        if (imagesLoaded) return;

        char playerDataPath[] = "Image//player_data.png";
        playerDataImg = iLoadImage(playerDataPath);
        imagesLoaded = true;
    }

    static void beginNewEntry()
    {
        teamName.clear();
        player1Name.clear();
        player2Name.clear();
        activeField = 0;

        for (int i = 0; i < 512; ++i)
        {
            keysWereDown[i] = false;
        }
    }

    static bool isComplete()
    {
        return !teamName.empty() &&
               !player1Name.empty() &&
               !player2Name.empty();
    }

    static bool updateInput()
    {
        if (playerDataImg == -1)
        {
            initialize();
        }

        bool startRequested = false;

        // The iGraphics keyboard system stores the current key state in
        // keyPressed[]. This edge check makes each physical key press add
        // only one character instead of repeating every timer tick.
        for (int key = 0; key < 512; ++key)
        {
            bool downNow = keyPressed[key] != 0;

            if (downNow && !keysWereDown[key])
            {
                if (key == 8 || key == 127) // Backspace / Delete-style code
                {
                    std::string& text = currentText();
                    if (!text.empty())
                    {
                        text.erase(text.size() - 1, 1);
                    }
                }
                else if (key == 9) // Tab: next input field
                {
                    activeField = (activeField + 1) % 3;
                }
                else if (key == 13) // Enter: next field or start
                {
                    if (activeField < 2)
                    {
                        activeField++;
                    }
                    else if (isComplete())
                    {
                        startRequested = true;
                    }
                }
                else if (isNameCharacter((unsigned char)key))
                {
                    std::string& text = currentText();
                    if ((int)text.size() < MAX_NAME_LENGTH)
                    {
                        text += (char)key;
                    }
                }
            }

            keysWereDown[key] = downNow;
        }

        return startRequested && isComplete();
    }

    static int handleClick(int mx, int my)
    {
        if (isInside(teamField, mx, my))
        {
            activeField = 0;
            return 0;
        }

        if (isInside(player1Field, mx, my))
        {
            activeField = 1;
            return 0;
        }

        if (isInside(player2Field, mx, my))
        {
            activeField = 2;
            return 0;
        }

        if (isInside(backButton, mx, my))
        {
            return 1; // Back
        }

        if (isInside(startButton, mx, my) && isComplete())
        {
            return 2; // Start Game
        }

        return 0;
    }

    static void draw()
    {
        if (playerDataImg == -1)
        {
            initialize();
        }

        iShowImage(0, 0, 1920, 980, playerDataImg);

        // Active-field outline helps show where keyboard input is going.
        iSetColor(255, 215, 90);
        if (activeField == 0)
        {
            iRectangle(teamField.x1, teamField.y1,
                       teamField.x2 - teamField.x1,
                       teamField.y2 - teamField.y1);
        }
        else if (activeField == 1)
        {
            iRectangle(player1Field.x1, player1Field.y1,
                       player1Field.x2 - player1Field.x1,
                       player1Field.y2 - player1Field.y1);
        }
        else
        {
            iRectangle(player2Field.x1, player2Field.y1,
                       player2Field.x2 - player2Field.x1,
                       player2Field.y2 - player2Field.y1);
        }

        iSetColor(45, 25, 10);
        drawText(teamField.x1 + 18, teamField.y1 + 20, teamName);
        drawText(player1Field.x1 + 18, player1Field.y1 + 20, player1Name);
        drawText(player2Field.x1 + 18, player2Field.y1 + 20, player2Name);

        // The provided player_data artwork has a Back control but no Start
        // button, so Enter (or the area beside Back) is used to continue.
        drawText(1240, 100, "Press ENTER to START GAME");

        if (!isComplete())
        {
            drawText(1240, 76, "Fill all three fields");
        }
    }
}

#endif
