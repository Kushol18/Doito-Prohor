#define _CRT_SECURE_NO_WARNINGS
#ifndef LEADERBOARD_HPP
#define LEADERBOARD_HPP

#include "iGraphics.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>

namespace Leaderboard
{
    struct Result
    {
        std::string teamName;
        std::string player1Name;
        std::string player2Name;
        int completionTimeSeconds;
    };

    struct Button
    {
        int x1;
        int y1;
        int x2;
        int y2;
    };

    static const Button backButton = { 760, 25, 1160, 170 };

    static int leaderboardImg = -1;
    static bool imagesLoaded = false;
    static bool savedPendingThisSession = false;
    static bool savedResultThisSession = false;
    static std::string pendingTeamName = "";
    static std::string pendingPlayer1Name = "";
    static std::string pendingPlayer2Name = "";
    static std::vector<Result> results;

    static const char* dataFileName()
    {
        return "player_data.txt";
    }

    static bool isInside(const Button& button, int mx, int my)
    {
        return mx >= button.x1 && mx <= button.x2 &&
               my >= button.y1 && my <= button.y2;
    }

    static void drawText(int x, int y, const std::string& text, void* font = GLUT_BITMAP_TIMES_ROMAN_24)
    {
        char buffer[256];
        std::strncpy(buffer, text.c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
        iText(x, y, buffer, font);
    }

    static void ensureDataFile()
    {
        std::ofstream file(dataFileName(), std::ios::app);
    }

    static bool parseResultLine(const std::string& line, Result& result)
    {
        std::stringstream stream(line);
        std::string completed;
        std::string secondsText;

        if (!std::getline(stream, completed, '|')) return false;
        if (!std::getline(stream, result.teamName, '|')) return false;
        if (!std::getline(stream, result.player1Name, '|')) return false;
        if (!std::getline(stream, result.player2Name, '|')) return false;
        if (!std::getline(stream, secondsText, '|')) return false;

        if (completed != "1") return false;

        std::stringstream timeStream(secondsText);
        if (!(timeStream >> result.completionTimeSeconds)) return false;
        if (result.completionTimeSeconds < 0) return false;

        return true;
    }

    static bool sortByTime(const Result& a, const Result& b)
    {
        return a.completionTimeSeconds < b.completionTimeSeconds;
    }

    static void initialize()
    {
        if (!imagesLoaded)
        {
            char leaderboardPath[] = "Image//leaderboards.png";
            leaderboardImg = iLoadImage(leaderboardPath);
            imagesLoaded = true;
        }

        ensureDataFile();
    }

    static void resetSessionSaveState()
    {
        savedPendingThisSession = false;
        savedResultThisSession = false;
        pendingTeamName.clear();
        pendingPlayer1Name.clear();
        pendingPlayer2Name.clear();
    }

    // Records the submitted player information when a new game actually
    // starts. The completion status is 0, so this entry is never eligible
    // for the Top 5 leaderboard until a later completed record is written.
    static void savePendingPlayerInfo(const std::string& teamName,
                                      const std::string& player1Name,
                                      const std::string& player2Name)
    {
        if (savedPendingThisSession)
        {
            return;
        }

        ensureDataFile();

        std::ofstream file(dataFileName(), std::ios::app);
        if (!file)
        {
            return;
        }

        // Format: completed|team|player1|player2|completion_seconds
        file << "0|"
             << teamName << "|"
             << player1Name << "|"
             << player2Name << "|-1\n";

        savedPendingThisSession = true;
        pendingTeamName = teamName;
        pendingPlayer1Name = player1Name;
        pendingPlayer2Name = player2Name;
    }

    static bool updatePendingResult(int completionTimeSeconds)
    {
        if (!savedPendingThisSession || completionTimeSeconds < 0)
        {
            return false;
        }

        std::ifstream input(dataFileName());
        if (!input)
        {
            return false;
        }

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(input, line))
        {
            lines.push_back(line);
        }
        input.close();

        // Replace the most recent pending record for this exact gameplay
        // session instead of leaving the placeholder time (-1) in the file.
        for (int i = (int)lines.size() - 1; i >= 0; --i)
        {
            std::stringstream stream(lines[i]);
            std::string completed;
            std::string team;
            std::string player1;
            std::string player2;
            std::string oldTime;

            if (!std::getline(stream, completed, '|')) continue;
            if (!std::getline(stream, team, '|')) continue;
            if (!std::getline(stream, player1, '|')) continue;
            if (!std::getline(stream, player2, '|')) continue;
            if (!std::getline(stream, oldTime, '|')) continue;

            if (completed == "0" &&
                team == pendingTeamName &&
                player1 == pendingPlayer1Name &&
                player2 == pendingPlayer2Name &&
                oldTime == "-1")
            {
                std::stringstream replacement;
                replacement << "1|"
                            << pendingTeamName << "|"
                            << pendingPlayer1Name << "|"
                            << pendingPlayer2Name << "|"
                            << completionTimeSeconds;
                lines[i] = replacement.str();
                break;
            }
        }

        std::ofstream output(dataFileName(), std::ios::trunc);
        if (!output)
        {
            return false;
        }

        for (size_t i = 0; i < lines.size(); ++i)
        {
            output << lines[i] << "\n";
        }
        output.close();
        return true;
    }

    static void refresh()
    {
        ensureDataFile();
        results.clear();

        std::ifstream file(dataFileName());
        std::string line;

        while (std::getline(file, line))
        {
            Result result;
            if (parseResultLine(line, result))
            {
                results.push_back(result);
            }
        }

        std::sort(results.begin(), results.end(), sortByTime);
    }

    // Called when the future gameplay code has supplied both completion and
    // a real elapsed time. This does not decide when the game is completed.
    static void saveIfCompleted(bool gameCompletion,
                                int completionTimeSeconds,
                                const std::string& teamName,
                                const std::string& player1Name,
                                const std::string& player2Name)
    {
        if (!gameCompletion || savedResultThisSession)
        {
            return;
        }

        if (completionTimeSeconds < 0)
        {
            return;
        }

        ensureDataFile();

        // The current session already has a player-data line with a -1
        // placeholder. Replace that line with the real final time so the
        // active session never leaves an obsolete -1 completion time behind.
        if (updatePendingResult(completionTimeSeconds))
        {
            savedResultThisSession = true;
            refresh();
            return;
        }

        // Fallback for projects/data files created before pending-session
        // persistence was added.
        std::ofstream file(dataFileName(), std::ios::app);
        if (!file)
        {
            return;
        }

        // Format: completed|team|player1|player2|completion_seconds
        file << "1|"
             << teamName << "|"
             << player1Name << "|"
             << player2Name << "|"
             << completionTimeSeconds << "\n";

        savedResultThisSession = true;
        refresh();
    }

    static std::string formatTime(int seconds)
    {
        char buffer[32];
        int minutes = seconds / 60;
        int remainingSeconds = seconds % 60;
        std::sprintf(buffer, "%02d:%02d", minutes, remainingSeconds);
        return std::string(buffer);
    }

    static void draw()
    {
        if (leaderboardImg == -1)
        {
            initialize();
        }

        iShowImage(0, 0, 1920, 980, leaderboardImg);

        int rowY[5] = { 700, 580, 460, 340, 220 };
        int visibleCount = (int)results.size();
        if (visibleCount > 5) visibleCount = 5;

        iSetColor(45, 25, 10);
        for (int i = 0; i < visibleCount; ++i)
        {
            const Result& result = results[i];

            char positionText[16];
            std::sprintf(positionText, "%d", i + 1);
            iText(380, rowY[i], positionText, GLUT_BITMAP_TIMES_ROMAN_24);

            drawText(590, rowY[i], result.teamName);

            std::string players = result.player1Name + " / " + result.player2Name;
            drawText(1010, rowY[i], players);

            drawText(1490, rowY[i], formatTime(result.completionTimeSeconds));
        }
    }

    static bool handleClick(int mx, int my)
    {
        return isInside(backButton, mx, my);
    }
}

#endif
