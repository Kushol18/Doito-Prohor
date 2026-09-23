#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include <windows.h>

namespace Stopwatch
{
    static ULONGLONG startTick = 0;
    static ULONGLONG accumulatedMs = 0;
    static ULONGLONG pauseTick = 0;
    static bool running = false;
    static bool paused = false;
    static bool permanentlyStopped = false;

    inline void reset()
    {
        startTick = 0;
        accumulatedMs = 0;
        pauseTick = 0;
        running = false;
        paused = false;
        permanentlyStopped = false;
    }

    inline void start()
    {
        if (permanentlyStopped) return;
        accumulatedMs = 0;
        startTick = GetTickCount64();
        pauseTick = 0;
        running = true;
        paused = false;
    }

    inline void pause()
    {
        if (!running || paused || permanentlyStopped) return;
        ULONGLONG now = GetTickCount64();
        accumulatedMs += (now - startTick);
        pauseTick = now;
        paused = true;
    }

    inline void resume()
    {
        if (!running || !paused || permanentlyStopped) return;
        startTick = GetTickCount64();
        pauseTick = 0;
        paused = false;
    }

    inline void stopPermanent()
    {
        if (!running || permanentlyStopped) return;

        if (!paused)
        {
            ULONGLONG now = GetTickCount64();
            accumulatedMs += (now - startTick);
        }

        running = false;
        paused = false;
        pauseTick = 0;
        permanentlyStopped = true;
    }

    inline bool isRunning()
    {
        return running && !permanentlyStopped;
    }

    inline bool isPaused()
    {
        return paused;
    }

    inline int getElapsedSeconds()
    {
        ULONGLONG totalMs = accumulatedMs;
        if (running && !paused && !permanentlyStopped)
        {
            ULONGLONG now = GetTickCount64();
            totalMs += (now - startTick);
        }
        return (int)(totalMs / 1000ULL);
    }
}

#endif
